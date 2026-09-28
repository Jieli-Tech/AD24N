"""
串口数据波形显示工具（带GUI控制面板 · 自适应帧解析 · 循环分析 · AB符号可选 · 通道可命名 · 同步指示线 · 两点测量 · 锁定线数值显示）

帧结构（自适应识别）：
  1) 55 AA + N个通道字节    N自动推断
  2) 55 AB + 2*M字节        M个16位数(大端)，M自动推断，每通道可选有无符号

交互:
  - 鼠标滚轮          → 以光标为中心缩放
  - 鼠标移动          → 淡蓝虚线跟随
  - 左键单击          → 固定红色指示线（并在各子图曲线旁显示该帧Y值）
  - 右键单击          → 清除固定线 / 清除两点区间
  - 左键拖动          → 平移
  - 右键拖动          → 框选放大
  - Shift + 左键单击  → 选择测量区间（第1次=起点, 第2次=终点）
  - Esc               → 清除两点区间
  - R 键              → 重置视图
"""

import re
import os
from collections import Counter

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.widgets import SpanSelector
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import tkinter as tk
from tkinter import filedialog, messagebox

plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'Arial Unicode MS']
plt.rcParams['axes.unicode_minus'] = False

matplotlib.use('TkAgg')


# ==========================================================
#                  数据读取
# ==========================================================

def read_file_bytes(filepath):
    with open(filepath, 'rb') as f:
        return f.read()


def extract_bytes_from_text(text):
    tokens = re.findall(r'\b[0-9A-Fa-f]{2}\b', text)
    return [int(t, 16) for t in tokens]


def try_parse_as_text_hex(raw):
    try:
        text = raw.decode('ascii')
    except UnicodeDecodeError:
        return False, None
    stripped = re.sub(r'\s', '', text)
    if len(stripped) == 0:
        return False, None
    hex_ratio = len(re.findall(r'[0-9A-Fa-f]', stripped)) / len(stripped)
    if hex_ratio > 0.9:
        return True, extract_bytes_from_text(text)
    return False, None


# ==========================================================
#                  自适应帧解析
# ==========================================================

def find_all_headers(data):
    headers = []
    i = 0
    n = len(data)
    while i < n - 1:
        if data[i] == 0x55:
            if data[i + 1] == 0xAA:
                headers.append((i, 'AA'))
                i += 2
                continue
            elif data[i + 1] == 0xAB:
                headers.append((i, 'AB'))
                i += 2
                continue
        i += 1
    return headers


def payload_to_uint16_list(payload):
    vals = []
    for k in range(0, len(payload) - 1, 2):
        hi, lo = payload[k], payload[k + 1]
        vals.append((hi << 8) | lo)
    return vals


def convert_signed(raw_vals, signed_flags):
    out = []
    for v, s in zip(raw_vals, signed_flags):
        if s and v >= 0x8000:
            out.append(v - 0x10000)
        else:
            out.append(v)
    return out


def scan_frames(data, ab_signed_flags=None, log=print):
    headers = find_all_headers(data)
    log(f"检测到帧头总数: {len(headers)}")

    empty_result = {
        'aa': np.zeros((0, 1), dtype=np.int32),
        'ab': np.zeros((0, 1), dtype=np.int32),
        'aa_channels': 0,
        'ab_channels': 0,
        'aa_frames_raw': 0,
        'ab_frames_raw': 0,
        'header_total': 0,
    }
    if not headers:
        return empty_result

    aa_payloads = []
    ab_payload_lens = []
    ab_raw_lists = []
    aa_raw = 0
    ab_raw = 0

    for idx, (pos, kind) in enumerate(headers):
        payload_start = pos + 2
        if idx + 1 < len(headers):
            payload_end = headers[idx + 1][0]
        else:
            payload_end = len(data)
        payload = data[payload_start: payload_end]

        if kind == 'AA':
            aa_raw += 1
            aa_payloads.append(payload)
        elif kind == 'AB':
            ab_raw += 1
            ab_payload_lens.append(len(payload))
            ab_raw_lists.append(payload_to_uint16_list(payload))

    lengths = [len(p) for p in aa_payloads]
    if not lengths:
        aa_channels = 0
    else:
        cnt = Counter(lengths)
        aa_channels = cnt.most_common(1)[0][0]
        if aa_channels == 0:
            aa_channels = 1
        log(f"AA payload 长度分布: {dict(cnt)}")
        log(f"自动推断 55AA 通道数 N = {aa_channels}")

    aa_frames = []
    aa_dropped = 0
    for p in aa_payloads:
        if len(p) == aa_channels:
            aa_frames.append(p)
        else:
            aa_dropped += 1
    if aa_dropped > 0:
        log(f"⚠ 丢弃 {aa_dropped} 个长度不等于 {aa_channels} 的 AA 帧")

    if not ab_payload_lens:
        ab_channels = 0
    else:
        len_cnt = Counter(ab_payload_lens)
        most_len = len_cnt.most_common(1)[0][0]
        ab_channels = most_len // 2
        if ab_channels == 0:
            ab_channels = 1
        log(f"AB payload 长度分布: {dict(len_cnt)}")
        log(f"自动推断 55AB 数据个数 M = {ab_channels} (每帧 {ab_channels*2} 字节)")

    if ab_signed_flags is None or len(ab_signed_flags) != ab_channels:
        flags = [False] * ab_channels
    else:
        flags = list(ab_signed_flags)

    if ab_channels > 0:
        desc = ', '.join(f'AB{i+1}={"i16" if s else "u16"}'
                         for i, s in enumerate(flags))
        log(f"AB 通道符号设置: {desc}")

    ab_frames = []
    ab_dropped = 0
    for raw_vals in ab_raw_lists:
        if len(raw_vals) == ab_channels:
            ab_frames.append(convert_signed(raw_vals, flags))
        else:
            ab_dropped += 1
    if ab_dropped > 0:
        log(f"⚠ 丢弃 {ab_dropped} 个长度不等于 {ab_channels*2} 字节的 AB 帧")

    aa_arr = np.array(aa_frames, dtype=np.int32) if aa_frames \
        else np.zeros((0, aa_channels if aa_channels else 1), dtype=np.int32)
    ab_arr = np.array(ab_frames, dtype=np.int32) if ab_frames \
        else np.zeros((0, ab_channels if ab_channels else 1), dtype=np.int32)

    log(f"解析结果: 55AA 有效帧 {len(aa_frames)}/{aa_raw} 个, "
        f"55AB 有效帧 {len(ab_frames)}/{ab_raw} 个")

    return {
        'aa': aa_arr,
        'ab': ab_arr,
        'aa_channels': aa_channels,
        'ab_channels': ab_channels,
        'aa_frames_raw': aa_raw,
        'ab_frames_raw': ab_raw,
        'header_total': len(headers),
    }


def parse_file(filepath, ab_signed_flags=None, log=print):
    raw = read_file_bytes(filepath)
    log(f"文件: {filepath}")
    log(f"文件大小: {len(raw)} 字节")

    is_text, text_bytes = try_parse_as_text_hex(raw)
    if is_text:
        log("检测为【文本HEX格式】")
        data = text_bytes
    else:
        log("检测为【二进制数据】")
        data = list(raw)

    preview = ' '.join(f'{b:02X}' for b in data[:32])
    log(f"前32字节: {preview}")

    return scan_frames(data, ab_signed_flags=ab_signed_flags, log=log)


# ==========================================================
#                  主界面 App
# ==========================================================

class WaveApp:
    def __init__(self, root):
        self.root = root
        self.root.title("串口数据波形分析工具 · 自适应解析 · 循环监控 · "
                        "AB符号可选 · 通道可命名 · 指示线 · 两点测量")
        self.root.geometry("1500x1150")

        self.current_file = None
        self.result = None

        # 循环分析
        self.loop_var = tk.BooleanVar(value=False)
        self.interval_var = tk.StringVar(value="2")
        self.loop_job = None
        self._loop_busy = False

        # AB 勾选
        self.ab_signed_vars = []
        self.ab_flags_cache = []

        # 通道名称
        self.ch_names_cache = {'AA': [], 'AB': []}
        self.name_vars = {'AA': [], 'AB': []}

        # 指示线
        self.hover_lines = []
        self.hover_points = []
        self.pinned_lines = []
        self.pinned_points = []
        self.pinned_texts = []      # ★ 每个子图显示的 Y 值标签
        self.pinned_frame = None
        self._show_hover = tk.BooleanVar(value=True)

        # 两点测量
        self.measure_start = None
        self.measure_end = None
        self.measure_spans = []
        self.measure_vlines = []
        self._measure_shift = False

        # 拖动检测
        self._drag = {'pressed': False, 'moved': False,
                      'x0': 0, 'y0': 0, 'x0_data': 0, 'y0_data': 0,
                      'ax': None, 'xlim': None, 'ylim': None,
                      'button': None}

        # ---------------- 顶部控制面板 ----------------
        top = tk.Frame(root, padx=10, pady=8)
        top.pack(side=tk.TOP, fill=tk.X)

        self.btn_choose = tk.Button(
            top, text="选择文件", width=12, height=2,
            command=self.on_choose_file, bg='#e6f0ff')
        self.btn_choose.pack(side=tk.LEFT, padx=5)

        self.btn_analyze = tk.Button(
            top, text="分析新选文件", width=14, height=2,
            command=self.on_analyze, bg='#d9f5d9', state=tk.DISABLED)
        self.btn_analyze.pack(side=tk.LEFT, padx=5)

        loop_frame = tk.LabelFrame(top, text="循环分析", padx=8, pady=4)
        loop_frame.pack(side=tk.LEFT, padx=15)

        self.chk_loop = tk.Checkbutton(
            loop_frame, text="启用循环", variable=self.loop_var,
            command=self.on_toggle_loop, font=('Microsoft YaHei', 9))
        self.chk_loop.pack(side=tk.LEFT)

        tk.Label(loop_frame, text="间隔(秒):",
                 font=('Microsoft YaHei', 9)).pack(side=tk.LEFT, padx=(10, 2))

        self.entry_interval = tk.Entry(
            loop_frame, textvariable=self.interval_var,
            width=5, justify='center', font=('Consolas', 10))
        self.entry_interval.pack(side=tk.LEFT)

        self.lbl_loop_status = tk.Label(
            loop_frame, text="未运行", fg='#888',
            font=('Microsoft YaHei', 9))
        self.lbl_loop_status.pack(side=tk.LEFT, padx=8)

        # 指示线控件
        hint_frame = tk.LabelFrame(top, text="指示线", padx=8, pady=4)
        hint_frame.pack(side=tk.LEFT, padx=10)

        self.chk_hover = tk.Checkbutton(
            hint_frame, text="光标跟随线",
            variable=self._show_hover,
            command=self._on_toggle_hover,
            font=('Microsoft YaHei', 9))
        self.chk_hover.pack(side=tk.LEFT)

        self.lbl_pin = tk.Label(
            hint_frame, text="未锁定", fg='#888',
            font=('Microsoft YaHei', 9))
        self.lbl_pin.pack(side=tk.LEFT, padx=8)

        self.lbl_file = tk.Label(
            top, text="未选择文件", anchor='w', fg='#333',
            font=('Microsoft YaHei', 10))
        self.lbl_file.pack(side=tk.LEFT, padx=15, fill=tk.X, expand=True)

        # ---------------- AB 符号勾选面板 ----------------
        self.ab_chk_frame = tk.LabelFrame(
            root,
            text="AB 通道类型 (勾选=有符号 int16, 不勾=无符号 uint16)  "
                 "·  默认全部无符号",
            padx=8, pady=4)
        self.ab_chk_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=(0, 5))

        self.ab_chk_inner = tk.Frame(self.ab_chk_frame)
        self.ab_chk_inner.pack(side=tk.LEFT, fill=tk.X)

        tk.Label(self.ab_chk_inner,
                 text="(分析文件后自动生成 AB 通道勾选项)",
                 fg='#888', font=('Microsoft YaHei', 9)).pack(side=tk.LEFT)

        self.btn_apply_ab = tk.Button(
            self.ab_chk_frame, text="应用符号设置并重绘",
            command=self.on_apply_ab_flags,
            bg='#fff3cc', state=tk.DISABLED)
        self.btn_apply_ab.pack(side=tk.RIGHT, padx=5)

        # ---------------- 通道命名面板 ----------------
        self.name_frame = tk.LabelFrame(
            root,
            text="通道名称 (编辑后点【应用名称并重绘】生效)",
            padx=8, pady=4)
        self.name_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=(0, 5))

        self.name_inner = tk.Frame(self.name_frame)
        self.name_inner.pack(side=tk.LEFT, fill=tk.X)

        tk.Label(self.name_inner,
                 text="(分析文件后自动生成通道名称输入框)",
                 fg='#888', font=('Microsoft YaHei', 9)).pack(side=tk.LEFT)

        self.btn_apply_names = tk.Button(
            self.name_frame, text="应用名称并重绘",
            command=self.on_apply_names,
            bg='#e6e6ff', state=tk.DISABLED)
        self.btn_apply_names.pack(side=tk.RIGHT, padx=5)

        # ---------------- 两点测量面板 ----------------
        self.measure_frame = tk.LabelFrame(
            root,
            text="两点测量 (Shift + 左键 依次点两个点 · Esc 清除)",
            padx=8, pady=4)
        self.measure_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=(0, 5))

        self.measure_inner = tk.Frame(self.measure_frame)
        self.measure_inner.pack(side=tk.LEFT, fill=tk.X)

        self.lbl_measure = tk.Label(
            self.measure_inner,
            text="(未选择)  提示: 按住 Shift 后左键单击波形，选起点→终点",
            fg='#888', font=('Microsoft YaHei', 9), justify='left')
        self.lbl_measure.pack(side=tk.LEFT)

        self.btn_clear_measure = tk.Button(
            self.measure_frame, text="清除区间",
            command=self.clear_measure,
            bg='#ffe6e6', state=tk.DISABLED)
        self.btn_clear_measure.pack(side=tk.RIGHT, padx=5)

        # ---------------- 图形区域 ----------------
        self.fig = plt.figure(figsize=(13, 8))
        self.canvas = FigureCanvasTkAgg(self.fig, master=root)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # ---------------- 底部日志 ----------------
        bottom = tk.Frame(root, padx=8, pady=5)
        bottom.pack(side=tk.BOTTOM, fill=tk.X)

        tk.Label(bottom, text="运行日志:",
                 font=('Microsoft YaHei', 9, 'bold')) \
            .pack(side=tk.LEFT, anchor='n')

        self.log_box = tk.Text(bottom, height=6, wrap='word',
                               font=('Consolas', 9), bg='#f8f8f8')
        self.log_box.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

        self.axes = []
        self.span = None
        self._init_empty_plot()
        self._bind_interactions()

        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
        self.root.bind('<KeyPress-Shift_L>', lambda e: self._set_shift(True))
        self.root.bind('<KeyRelease-Shift_L>', lambda e: self._set_shift(False))
        self.root.bind('<KeyPress-Shift_R>', lambda e: self._set_shift(True))
        self.root.bind('<KeyRelease-Shift_R>', lambda e: self._set_shift(False))
        self.root.bind('<Escape>', lambda e: self.clear_measure())

        self.log("就绪。请点击【选择文件】。")
        self.log("交互: 左键单击锁定红色指示线（显示各通道Y值）; "
                 "Shift+左键两点测量; Esc清除区间")

    # ----------------------------------------------------
    def log(self, msg):
        self.log_box.insert(tk.END, str(msg) + "\n")
        self.log_box.see(tk.END)
        print(msg)

    # ----------------------------------------------------
    def _init_empty_plot(self):
        self.fig.clear()
        ax = self.fig.add_subplot(111)
        ax.text(0.5, 0.5, '请选择文件并点击【分析新选文件】',
                ha='center', va='center', fontsize=14, color='#888',
                transform=ax.transAxes)
        ax.set_xticks([])
        ax.set_yticks([])
        self.axes = [ax]
        self._clear_lines()
        self._clear_measure_graphics()
        self.fig.suptitle(
            '波形  [滚轮=缩放 | 移动=跟随 | 左键=锁定 | Shift+左键=两点测量 | Esc=清除]',
            fontsize=12)
        self.canvas.draw()

    def _clear_lines(self):
        self.hover_lines = []
        self.hover_points = []
        self.pinned_lines = []
        self.pinned_points = []
        self.pinned_texts = []

    def _clear_measure_graphics(self):
        self.measure_spans = []
        self.measure_vlines = []

    # ----------------------------------------------------
    def on_choose_file(self):
        filepath = filedialog.askopenfilename(
            title='请选择串口数据文件',
            filetypes=[
                ('数据文件 (*.dat *.txt)', '*.dat *.txt'),
                ('DAT 文件', '*.dat'),
                ('TXT 文件', '*.txt'),
                ('二进制文件', '*.bin *.log'),
                ('所有文件', '*.*'),
            ]
        )
        if not filepath:
            return
        self.current_file = filepath
        short = os.path.basename(filepath)
        self.lbl_file.config(text=f"已选择: {short}", fg='#0a5')
        self.btn_analyze.config(state=tk.NORMAL)
        self.log(f"已选择文件: {filepath}")

    # ----------------------------------------------------
    def on_analyze(self):
        if not self.current_file:
            messagebox.showwarning("提示", "请先选择一个文件！")
            return
        self._do_analyze(silent=False)

    def _do_analyze(self, silent=False):
        if self._loop_busy:
            return
        self._loop_busy = True
        try:
            if not silent:
                self.log("=" * 55)
                self.log("开始分析...")

            flags = self._get_current_flags()

            try:
                self.result = parse_file(self.current_file,
                                         ab_signed_flags=flags,
                                         log=self.log)
            except Exception as e:
                self.log(f"❌ 解析失败: {e}")
                if not silent:
                    messagebox.showerror("错误", f"解析失败:\n{e}")
                return

            aa = self.result['aa']
            ab = self.result['ab']

            if len(aa) == 0 and len(ab) == 0:
                self.log("⚠ 没有解析到任何帧")
                if not silent:
                    messagebox.showwarning("提示",
                                           "没有解析到有效数据，请检查文件格式。")
                return

            self.log(f"55AA 数据形状: {aa.shape}  "
                     f"(自动推断通道数 N={self.result['aa_channels']})")
            self.log(f"55AB 数据形状: {ab.shape}  "
                     f"(自动推断数据个数 M={self.result['ab_channels']})")

            self._rebuild_ab_checkboxes(self.result['ab_channels'])
            self._rebuild_name_inputs(self.result['aa_channels'],
                                      self.result['ab_channels'])

            cur_flags = self._get_current_flags()
            need_reparse = False
            if cur_flags is not None \
                    and len(cur_flags) == self.result['ab_channels']:
                if flags != cur_flags:
                    need_reparse = True
            elif flags is None and self.result['ab_channels'] > 0:
                need_reparse = True

            if need_reparse:
                self.result = parse_file(self.current_file,
                                         ab_signed_flags=cur_flags,
                                         log=lambda m: None)
                ab = self.result['ab']

            self.draw_wave(aa, ab)
            self.log("✅ 完成")
        finally:
            self._loop_busy = False

    # ----------------------------------------------------
    def _get_current_flags(self):
        if self.ab_signed_vars:
            return [v.get() for v in self.ab_signed_vars]
        if self.ab_flags_cache:
            return list(self.ab_flags_cache)
        return None

    def _rebuild_ab_checkboxes(self, m):
        if m == len(self.ab_signed_vars):
            self.btn_apply_ab.config(
                state=tk.NORMAL if m > 0 else tk.DISABLED)
            return

        old_flags = self._get_current_flags() or []

        for w in self.ab_chk_inner.winfo_children():
            w.destroy()
        self.ab_signed_vars = []

        if m <= 0:
            tk.Label(self.ab_chk_inner,
                     text="(文件中未检测到 55AB 帧)",
                     fg='#888', font=('Microsoft YaHei', 9)) \
                .pack(side=tk.LEFT)
            self.btn_apply_ab.config(state=tk.DISABLED)
            self.ab_flags_cache = []
            return

        tk.Label(self.ab_chk_inner, text=f"检测到 {m} 个 AB 数据:",
                 font=('Microsoft YaHei', 9, 'bold')) \
            .pack(side=tk.LEFT, padx=(0, 8))

        for i in range(m):
            var = tk.BooleanVar(
                value=(old_flags[i] if i < len(old_flags) else False))
            self.ab_signed_vars.append(var)
            cb = tk.Checkbutton(
                self.ab_chk_inner, text=f"AB-{i+1} 有符号",
                variable=var, font=('Microsoft YaHei', 9))
            cb.pack(side=tk.LEFT, padx=3)

        self.btn_apply_ab.config(state=tk.NORMAL)
        self.ab_flags_cache = [v.get() for v in self.ab_signed_vars]

    # ----------------------------------------------------
    def _default_name(self, kind, idx):
        return f'{kind}-{idx+1}'

    def _rebuild_name_inputs(self, n_aa, n_ab):
        cur_aa = len(self.name_vars['AA'])
        cur_ab = len(self.name_vars['AB'])

        if n_aa == cur_aa and n_ab == cur_ab:
            self.btn_apply_names.config(
                state=tk.NORMAL if (n_aa + n_ab) > 0 else tk.DISABLED)
            return

        old_aa = [v.get() for v in self.name_vars['AA']]
        old_ab = [v.get() for v in self.name_vars['AB']]

        for w in self.name_inner.winfo_children():
            w.destroy()
        self.name_vars = {'AA': [], 'AB': []}

        if n_aa + n_ab == 0:
            tk.Label(self.name_inner,
                     text="(文件中未检测到任何通道)",
                     fg='#888', font=('Microsoft YaHei', 9)) \
                .pack(side=tk.LEFT)
            self.btn_apply_names.config(state=tk.DISABLED)
            return

        if n_aa > 0:
            tk.Label(self.name_inner, text="AA:",
                     font=('Microsoft YaHei', 9, 'bold')) \
                .pack(side=tk.LEFT, padx=(0, 4))
            for i in range(n_aa):
                name = old_aa[i] if i < len(old_aa) \
                    else self._default_name('AA', i)
                var = tk.StringVar(value=name)
                self.name_vars['AA'].append(var)
                tk.Label(self.name_inner, text=f"CH{i+1}",
                         font=('Microsoft YaHei', 8)) \
                    .pack(side=tk.LEFT)
                tk.Entry(self.name_inner, textvariable=var,
                         width=8, justify='center',
                         font=('Consolas', 9)) \
                    .pack(side=tk.LEFT, padx=(1, 6))

        if n_ab > 0:
            tk.Label(self.name_inner, text="AB:",
                     font=('Microsoft YaHei', 9, 'bold')) \
                .pack(side=tk.LEFT, padx=(10, 4))
            for i in range(n_ab):
                name = old_ab[i] if i < len(old_ab) \
                    else self._default_name('AB', i)
                var = tk.StringVar(value=name)
                self.name_vars['AB'].append(var)
                tk.Label(self.name_inner, text=f"CH{i+1}",
                         font=('Microsoft YaHei', 8)) \
                    .pack(side=tk.LEFT)
                tk.Entry(self.name_inner, textvariable=var,
                         width=8, justify='center',
                         font=('Consolas', 9)) \
                    .pack(side=tk.LEFT, padx=(1, 6))

        self.ch_names_cache['AA'] = [v.get() for v in self.name_vars['AA']]
        self.ch_names_cache['AB'] = [v.get() for v in self.name_vars['AB']]
        self.btn_apply_names.config(state=tk.NORMAL)

    def _get_channel_names(self):
        names = {'AA': [], 'AB': []}
        for kind in ('AA', 'AB'):
            if self.name_vars[kind]:
                names[kind] = [v.get().strip() or
                               self._default_name(kind, i)
                               for i, v in enumerate(self.name_vars[kind])]
            elif self.ch_names_cache[kind]:
                names[kind] = list(self.ch_names_cache[kind])
        return names

    def on_apply_names(self):
        if self.result is None:
            messagebox.showwarning("提示", "请先分析一个文件！")
            return

        names = self._get_channel_names()
        self.ch_names_cache = {
            'AA': list(names['AA']),
            'AB': list(names['AB']),
        }
        self.log("=" * 55)
        self.log("应用通道名称...")
        self.log(f"AA 名称: {names['AA']}")
        self.log(f"AB 名称: {names['AB']}")
        self.draw_wave(self.result['aa'], self.result['ab'])
        self.log("✅ 已重绘")

    # ----------------------------------------------------
    def on_apply_ab_flags(self):
        if not self.current_file or self.result is None:
            messagebox.showwarning("提示", "请先分析一个文件！")
            return
        if self.result['ab_channels'] <= 0:
            return

        flags = self._get_current_flags()
        self.ab_flags_cache = list(flags) if flags else []

        self.log("=" * 55)
        self.log("应用 AB 符号设置...")
        try:
            self.result = parse_file(self.current_file,
                                     ab_signed_flags=flags,
                                     log=self.log)
        except Exception as e:
            self.log(f"❌ 重解析失败: {e}")
            return
        self.draw_wave(self.result['aa'], self.result['ab'])
        self.log("✅ 已重绘")

    # ----------------------------------------------------
    #              循环分析
    # ----------------------------------------------------
    def on_toggle_loop(self):
        if self.loop_var.get():
            if not self.current_file:
                messagebox.showwarning("提示", "请先选择文件！")
                self.loop_var.set(False)
                return
            interval = self._get_interval()
            if interval is None:
                messagebox.showerror("错误", "间隔秒数无效，请输入正数！")
                self.loop_var.set(False)
                return
            self._start_loop()
        else:
            self._stop_loop()

    def _get_interval(self):
        try:
            v = float(self.interval_var.get().strip())
            if v <= 0:
                return None
            return v
        except Exception:
            return None

    def _start_loop(self):
        interval = self._get_interval()
        if interval is None:
            messagebox.showerror("错误", "间隔秒数无效！")
            self.loop_var.set(False)
            return

        self.entry_interval.config(state=tk.DISABLED)
        self.lbl_loop_status.config(text=f"运行中 (每 {interval}s)", fg='#0a0')

        self.log("=" * 55)
        self.log(f"🔁 开始循环分析，间隔 {interval} 秒")
        self._loop_tick()

    def _loop_tick(self):
        if not self.loop_var.get():
            return
        interval = self._get_interval()
        if interval is None:
            self._stop_loop()
            return

        self.log(f"--- 循环分析 @ {self._now_str()} ---")
        self._do_analyze(silent=True)

        self.loop_job = self.root.after(int(interval * 1000), self._loop_tick)

    def _stop_loop(self):
        if self.loop_job is not None:
            try:
                self.root.after_cancel(self.loop_job)
            except Exception:
                pass
            self.loop_job = None

        self.entry_interval.config(state=tk.NORMAL)
        self.lbl_loop_status.config(text="未运行", fg='#888')

        if self.loop_var.get():
            self.loop_var.set(False)
        self.log("⏹ 已停止循环分析")

    @staticmethod
    def _now_str():
        import datetime
        return datetime.datetime.now().strftime('%H:%M:%S')

    def on_close(self):
        if self.loop_job is not None:
            try:
                self.root.after_cancel(self.loop_job)
            except Exception:
                pass
        self.root.destroy()

    # ----------------------------------------------------
    def draw_wave(self, aa_data, ab_data, max_points=100000):
        has_aa = len(aa_data) > 0
        has_ab = len(ab_data) > 0

        self.fig.clear()
        self.axes = []
        self._clear_lines()
        self._clear_measure_graphics()
        self.pinned_frame = None

        n_rows = 0
        if has_aa:
            n_rows += aa_data.shape[1]
        if has_ab:
            n_rows += ab_data.shape[1]

        if n_rows == 0:
            self._init_empty_plot()
            return

        colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728',
                  '#9467bd', '#8c564b', '#e377c2', '#7f7f7f',
                  '#17becf', '#bcbd22']

        gs = self.fig.add_gridspec(n_rows, 1, hspace=0.18,
                                   top=0.93, bottom=0.07,
                                   left=0.09, right=0.97)

        names = self._get_channel_names()
        row = 0
        self._x_max = 0

        if has_aa:
            n_aa_frames = aa_data.shape[0]
            self._x_max = max(self._x_max, n_aa_frames - 1)
            if n_aa_frames > max_points:
                step = n_aa_frames // max_points
                aa_plot = aa_data[::step]
                x_aa = np.arange(0, n_aa_frames, step)
            else:
                aa_plot = aa_data
                x_aa = np.arange(n_aa_frames)

            for ch in range(aa_data.shape[1]):
                ax = self.fig.add_subplot(gs[row, 0])
                self.axes.append(ax)
                c = colors[ch % len(colors)]
                ax.plot(x_aa, aa_plot[:, ch], color=c, linewidth=0.8)

                ch_name = names['AA'][ch] if ch < len(names['AA']) \
                    else f'AA-{ch+1}'
                ax.set_ylabel(ch_name, color=c)
                ax.grid(True, alpha=0.3)

                ymin = int(aa_data[:, ch].min())
                ymax = int(aa_data[:, ch].max())
                margin = max(5, (ymax - ymin) * 0.1)
                ax.set_ylim(ymin - margin, ymax + margin)

                stats = (f"min={ymin}  max={ymax}  "
                         f"mean={aa_data[:, ch].mean():.1f}  "
                         f"帧数={n_aa_frames}")
                ax.text(0.01, 0.95, stats, transform=ax.transAxes,
                        ha='left', va='top', fontsize=9,
                        bbox=dict(boxstyle='round',
                                  facecolor='white', alpha=0.8))
                row += 1

        if has_ab:
            n_ab_frames = ab_data.shape[0]
            self._x_max = max(self._x_max, n_ab_frames - 1)
            if n_ab_frames > max_points:
                step = n_ab_frames // max_points
                ab_plot = ab_data[::step]
                x_ab = np.arange(0, n_ab_frames, step)
            else:
                ab_plot = ab_data
                x_ab = np.arange(n_ab_frames)

            flags = self._get_current_flags() or [False] * ab_data.shape[1]

            for ch in range(ab_data.shape[1]):
                ax = self.fig.add_subplot(gs[row, 0])
                self.axes.append(ax)
                c = colors[ch % len(colors)]
                ax.plot(x_ab, ab_plot[:, ch], color=c, linewidth=0.9)

                is_signed = flags[ch] if ch < len(flags) else False
                tag = 'i16' if is_signed else 'u16'
                ch_name = names['AB'][ch] if ch < len(names['AB']) \
                    else f'AB-{ch+1}'
                ax.set_ylabel(f'{ch_name}({tag})', color=c)
                ax.grid(True, alpha=0.3)

                ymin = int(ab_data[:, ch].min())
                ymax = int(ab_data[:, ch].max())
                margin = max(1, (ymax - ymin) * 0.1)
                ax.set_ylim(ymin - margin, ymax + margin)

                if is_signed:
                    ax.axhline(0, color='gray',
                               linewidth=0.6, linestyle='--')

                stats = (f"min={ymin}  max={ymax}  "
                         f"mean={ab_data[:, ch].mean():.1f}  "
                         f"帧数={n_ab_frames}")
                ax.text(0.01, 0.95, stats, transform=ax.transAxes,
                        ha='left', va='top', fontsize=9,
                        bbox=dict(boxstyle='round',
                                  facecolor='white', alpha=0.8))
                row += 1

        if len(self.axes) > 1:
            for a in self.axes[:-1]:
                a.set_xticklabels([])

        self.axes[-1].set_xlabel('帧序号')

        self._current_aa = aa_data
        self._current_ab = ab_data
        self._xlim_full = (0, max(self._x_max, 1))

        n_aa_ch = self.result['aa_channels'] if self.result else 0
        n_ab_ch = self.result['ab_channels'] if self.result else 0
        title = f'波形  {os.path.basename(self.current_file)}  |  '
        title += f'55AA: {aa_data.shape[0]}帧 (N={n_aa_ch})'
        if has_ab:
            title += f'  |  55AB: {n_ab_frames}帧 (M={n_ab_ch})'
        title += ('   [滚轮=缩放 | 移动=跟随 | 左键=锁定(显示Y值) | '
                  'Shift+左键=两点测量 | Esc=清除]')
        self.fig.suptitle(title, fontsize=11)

        self.canvas.draw()
        self._rebind_span()

        if self.measure_start is not None and self.measure_end is not None:
            self._redraw_measure()

    # ----------------------------------------------------
    #          ★ 指示线 / 两点测量
    # ----------------------------------------------------
    def _create_hover_line_on_ax(self, ax):
        line = ax.axvline(0, color='#3399ff', lw=1.0,
                          ls='--', alpha=0.7, visible=False)
        point, = ax.plot([], [], 'o', color='#3399ff',
                         markersize=5, visible=False)
        return line, point

    def _create_pinned_line_on_ax(self, ax):
        """创建红色指示线 + 红点 + ★ Y值文本标签"""
        line = ax.axvline(0, color='red', lw=1.2,
                          ls='--', alpha=0.9, visible=False)
        point, = ax.plot([], [], 'o', color='red',
                         markersize=6, visible=False)
        # Y 值标签：初始隐藏，位置后续动态设置
        text = ax.text(0, 0, '', fontsize=9, color='red',
                       ha='left', va='bottom', visible=False,
                       bbox=dict(boxstyle='round,pad=0.25',
                                 facecolor='#fff5f5',
                                 edgecolor='red', alpha=0.9))
        return line, point, text

    def _update_hover(self, xdata):
        if not self._show_hover.get() or not self.axes:
            for l in self.hover_lines:
                l.set_visible(False)
            for p in self.hover_points:
                p.set_visible(False)
            self.canvas.draw_idle()
            return

        for i, ax in enumerate(self.axes):
            line = self.hover_lines[i]
            point = self.hover_points[i]
            line.set_xdata([xdata, xdata])
            line.set_visible(True)

            yv = self._get_y_at(ax, xdata)
            if yv is not None:
                point.set_data([xdata], [yv])
                point.set_visible(True)
            else:
                point.set_visible(False)

        self.canvas.draw_idle()

    def _get_y_at(self, ax, xdata):
        try:
            idx = int(round(xdata))
        except Exception:
            return None

        n_aa = self.result['aa_channels'] if self.result else 0
        i = self.axes.index(ax)

        if i < n_aa:
            data = self._current_aa
            ch = i
        else:
            data = self._current_ab
            ch = i - n_aa

        if data is None or len(data) == 0:
            return None
        if idx < 0 or idx >= data.shape[0]:
            return None
        if ch >= data.shape[1]:
            return None
        return int(data[idx, ch])

    def _pin_line(self, xdata):
        if not self.axes:
            return
        x = int(round(xdata))
        if self._xlim_full and (x < self._xlim_full[0]
                                 or x > self._xlim_full[1]):
            return

        self.pinned_frame = x

        for i, ax in enumerate(self.axes):
            line = self.pinned_lines[i]
            point = self.pinned_points[i]
            text = self.pinned_texts[i]

            line.set_xdata([x, x])
            line.set_visible(True)

            yv = self._get_y_at(ax, x)
            if yv is not None:
                point.set_data([x], [yv])
                point.set_visible(True)

                # ★ 更新文本标签：显示 "帧x = y值"
                text.set_position((x, yv))
                text.set_text(f'x={x}\ny={yv}')
                text.set_visible(True)
            else:
                point.set_visible(False)
                text.set_visible(False)

        self.lbl_pin.config(text=f"锁定: 帧 {x}", fg='red')
        self.canvas.draw_idle()

        # 日志输出该帧所有通道数值
        self.log(f"📌 锁定帧 {x}:")
        self._log_frame_values(x)

    def _log_frame_values(self, x):
        """输出指定帧各通道的数值"""
        names = self._get_channel_names()
        n_aa = self.result['aa_channels'] if self.result else 0
        n_ab = self.result['ab_channels'] if self.result else 0

        if n_aa > 0 and self._current_aa is not None \
                and 0 <= x < len(self._current_aa):
            for ch in range(n_aa):
                cname = names['AA'][ch] if ch < len(names['AA']) \
                    else f'AA-{ch+1}'
                self.log(f"  [{cname}] = {int(self._current_aa[x, ch])}")

        if n_ab > 0 and self._current_ab is not None \
                and 0 <= x < len(self._current_ab):
            for ch in range(n_ab):
                cname = names['AB'][ch] if ch < len(names['AB']) \
                    else f'AB-{ch+1}'
                self.log(f"  [{cname}] = {int(self._current_ab[x, ch])}")

    def _clear_pinned(self):
        for l in self.pinned_lines:
            l.set_visible(False)
        for p in self.pinned_points:
            p.set_visible(False)
        for t in self.pinned_texts:
            t.set_visible(False)
        self.pinned_frame = None
        self.lbl_pin.config(text="未锁定", fg='#888')
        self.canvas.draw_idle()

    def _on_toggle_hover(self):
        if not self._show_hover.get():
            for l in self.hover_lines:
                l.set_visible(False)
            for p in self.hover_points:
                p.set_visible(False)
            self.canvas.draw_idle()

    def _set_shift(self, val):
        self._measure_shift = val

    # ---------------- 两点测量 ----------------
    def _handle_measure_click(self, xdata):
        if not self.axes or xdata is None:
            return
        x = int(round(xdata))
        if self._xlim_full and (x < self._xlim_full[0]
                                 or x > self._xlim_full[1]):
            return

        if self.measure_start is None:
            self.measure_start = x
            self.measure_end = None
            self.lbl_measure.config(
                text=f"起点: 帧 {x}   等待选择终点... (再按住Shift左键点第二个点)",
                fg='#c60', justify='left')
            self.log(f"🎯 测量起点: 帧 {x}")
            self._redraw_measure()
        else:
            self.measure_end = x
            self._redraw_measure()
            self._update_measure_label()
            self.log(f"🎯 测量终点: 帧 {x}")
            self._log_measure_stats()

    def _update_measure_label(self):
        s, e = self.measure_start, self.measure_end
        if s is None:
            self.lbl_measure.config(text="(未选择)", fg='#888')
            self.btn_clear_measure.config(state=tk.DISABLED)
            return
        if e is None:
            self.lbl_measure.config(
                text=f"起点: 帧 {s}   等待选择终点...",
                fg='#c60', justify='left')
            self.btn_clear_measure.config(state=tk.NORMAL)
            return

        lo, hi = sorted([s, e])
        n = hi - lo + 1
        self.lbl_measure.config(
            text=f"区间: 帧 {lo} ~ {hi}   数据量 = {n} 帧   "
                 f"(清除请按 Esc 或点【清除区间】)",
            fg='#000', justify='left')
        self.btn_clear_measure.config(state=tk.NORMAL)

    def _redraw_measure(self):
        for sp in self.measure_spans:
            try:
                sp.remove()
            except Exception:
                pass
        for vl in self.measure_vlines:
            for v in vl:
                try:
                    v.remove()
                except Exception:
                    pass
        self.measure_spans = []
        self.measure_vlines = []

        s, e = self.measure_start, self.measure_end
        if s is None:
            self.canvas.draw_idle()
            return

        if e is None:
            for ax in self.axes:
                v = ax.axvline(s, color='#ff9900', lw=1.2,
                               ls='-', alpha=0.8)
                self.measure_vlines.append([v])
            self.canvas.draw_idle()
            return

        lo, hi = sorted([s, e])
        for ax in self.axes:
            span = ax.axvspan(lo, hi, color='#ffee88', alpha=0.35,
                              zorder=0)
            v1 = ax.axvline(lo, color='#ff9900', lw=1.0,
                            ls='-', alpha=0.8)
            v2 = ax.axvline(hi, color='#ff9900', lw=1.0,
                            ls='-', alpha=0.8)
            self.measure_spans.append(span)
            self.measure_vlines.append([v1, v2])

        self.canvas.draw_idle()

    def _log_measure_stats(self):
        s, e = self.measure_start, self.measure_end
        if s is None or e is None:
            return
        lo, hi = sorted([s, e])
        n = hi - lo + 1
        self.log(f"📊 测量区间: 帧 {lo} ~ {hi}, 数据量 = {n} 帧")

        n_aa = self.result['aa_channels'] if self.result else 0
        names = self._get_channel_names()

        if n_aa > 0 and self._current_aa is not None and len(self._current_aa) > 0:
            seg = self._current_aa[lo:hi+1]
            for ch in range(min(n_aa, seg.shape[1])):
                cname = names['AA'][ch] if ch < len(names['AA']) else f'AA-{ch+1}'
                self.log(
                    f"  [{cname}] 起点={self._current_aa[lo, ch]}  "
                    f"终点={self._current_aa[hi, ch]}  "
                    f"Δ={int(self._current_aa[hi, ch]) - int(self._current_aa[lo, ch]):+d}  "
                    f"min={int(seg[:, ch].min())}  max={int(seg[:, ch].max())}  "
                    f"mean={seg[:, ch].mean():.2f}")

        n_ab = self.result['ab_channels'] if self.result else 0
        if n_ab > 0 and self._current_ab is not None and len(self._current_ab) > 0:
            seg = self._current_ab[lo:hi+1]
            for ch in range(min(n_ab, seg.shape[1])):
                cname = names['AB'][ch] if ch < len(names['AB']) else f'AB-{ch+1}'
                self.log(
                    f"  [{cname}] 起点={self._current_ab[lo, ch]}  "
                    f"终点={self._current_ab[hi, ch]}  "
                    f"Δ={int(self._current_ab[hi, ch]) - int(self._current_ab[lo, ch]):+d}  "
                    f"min={int(seg[:, ch].min())}  max={int(seg[:, ch].max())}  "
                    f"mean={seg[:, ch].mean():.2f}")

    def clear_measure(self):
        self.measure_start = None
        self.measure_end = None
        for sp in self.measure_spans:
            try:
                sp.remove()
            except Exception:
                pass
        for vl in self.measure_vlines:
            for v in vl:
                try:
                    v.remove()
                except Exception:
                    pass
        self.measure_spans = []
        self.measure_vlines = []
        self.lbl_measure.config(
            text="(未选择)  提示: 按住 Shift 后左键单击波形，选起点→终点",
            fg='#888', justify='left')
        self.btn_clear_measure.config(state=tk.DISABLED)
        self.canvas.draw_idle()
        self.log("🧹 已清除测量区间")

    # ----------------------------------------------------
    def _bind_interactions(self):
        canvas = self.canvas

        def _ensure_lines():
            if len(self.hover_lines) != len(self.axes):
                self.hover_lines = []
                self.hover_points = []
                self.pinned_lines = []
                self.pinned_points = []
                self.pinned_texts = []
                for ax in self.axes:
                    hl, hp = self._create_hover_line_on_ax(ax)
                    pl, pp, pt = self._create_pinned_line_on_ax(ax)
                    self.hover_lines.append(hl)
                    self.hover_points.append(hp)
                    self.pinned_lines.append(pl)
                    self.pinned_points.append(pp)
                    self.pinned_texts.append(pt)

        def on_scroll(event):
            _ensure_lines()
            if event.inaxes is None:
                return
            ax = event.inaxes
            xdata, ydata = event.xdata, event.ydata
            scale = 1.2 if event.button == 'down' else 1 / 1.2
            xlim = ax.get_xlim()
            ylim = ax.get_ylim()
            new_xlim = (xdata + (xlim[0] - xdata) * scale,
                        xdata + (xlim[1] - xdata) * scale)
            new_ylim = (ydata + (ylim[0] - ydata) * scale,
                        ydata + (ylim[1] - ydata) * scale)
            for a in self.axes:
                a.set_xlim(new_xlim)
            ax.set_ylim(new_ylim)
            canvas.draw_idle()

        def on_press(event):
            _ensure_lines()
            if event.inaxes is None:
                return
            self._drag['pressed'] = True
            self._drag['moved'] = False
            self._drag['x0'] = event.x
            self._drag['y0'] = event.y
            self._drag['x0_data'] = event.xdata
            self._drag['y0_data'] = event.ydata
            self._drag['ax'] = event.inaxes
            self._drag['xlim'] = event.inaxes.get_xlim()
            self._drag['ylim'] = event.inaxes.get_ylim()
            self._drag['button'] = event.button

            if event.button == 3:
                self._clear_pinned()

        def on_release(event):
            if not self._drag['pressed']:
                return
            dx = abs(event.x - self._drag['x0'])
            dy = abs(event.y - self._drag['y0'])
            is_click = (dx < 4 and dy < 4)

            if is_click and self._drag['button'] == 1 \
                    and event.xdata is not None:
                if self._measure_shift:
                    self._handle_measure_click(event.xdata)
                else:
                    self._pin_line(event.xdata)

            self._drag['pressed'] = False
            self._drag['moved'] = False

        def on_motion(event):
            _ensure_lines()
            if event.inaxes is None:
                for l in self.hover_lines:
                    l.set_visible(False)
                for p in self.hover_points:
                    p.set_visible(False)
                canvas.draw_idle()
                return

            if event.xdata is not None:
                self._update_hover(event.xdata)

            if not self._drag['pressed']:
                return

            dx_pix = abs(event.x - self._drag['x0'])
            dy_pix = abs(event.y - self._drag['y0'])
            if dx_pix > 4 or dy_pix > 4:
                self._drag['moved'] = True

            if not self._drag['moved']:
                return
            if self._drag['button'] != 1:
                return
            if event.xdata is None or event.ydata is None:
                return

            dx = event.xdata - self._drag['x0_data']
            dy = event.ydata - self._drag['y0_data']
            new_xlim = (self._drag['xlim'][0] - dx,
                        self._drag['xlim'][1] - dx)
            new_ylim = (self._drag['ylim'][0] - dy,
                        self._drag['ylim'][1] - dy)
            for a in self.axes:
                a.set_xlim(new_xlim)
            self._drag['ax'].set_ylim(new_ylim)
            canvas.draw_idle()

        def on_key(event):
            if event.key and event.key.lower() == 'r' \
                    and self.result is not None:
                self.draw_wave(self.result['aa'], self.result['ab'])
            elif event.key == 'escape':
                self.clear_measure()

        canvas.mpl_connect('scroll_event', on_scroll)
        canvas.mpl_connect('button_press_event', on_press)
        canvas.mpl_connect('button_release_event', on_release)
        canvas.mpl_connect('motion_notify_event', on_motion)
        canvas.mpl_connect('key_press_event', on_key)

        self._ensure_lines = _ensure_lines

    def _rebind_span(self):
        if not self.axes:
            return
        if self.span is not None:
            try:
                self.span.disconnect_events()
            except Exception:
                pass
        self.span = SpanSelector(
            self.axes[0], self._on_select_cb, 'horizontal',
            useblit=True, button=3,
            props=dict(alpha=0.3, facecolor='yellow'),
            interactive=True)

    def _on_select_cb(self, eclick, erelease):
        if eclick.inaxes is None or erelease.inaxes is None:
            return
        x1, x2 = sorted([eclick.xdata, erelease.xdata])
        y1, y2 = sorted([eclick.ydata, erelease.ydata])
        if abs(x2 - x1) < 1e-6:
            return
        for a in self.axes:
            a.set_xlim(x1, x2)
        if abs(y2 - y1) > 1e-6:
            eclick.inaxes.set_ylim(y1, y2)
        self.canvas.draw_idle()


# ==========================================================
#                  入口
# ==========================================================

def main():
    root = tk.Tk()
    app = WaveApp(root)
    root.mainloop()


if __name__ == '__main__':
    main()