# 总的 Makefile，用于调用目录下各个子工程对应的 Makefile
# 注意： Linux 下编译方式：
# 1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
# 2. 下载后，解压到 /opt/jieli 目录下，保证
#   /opt/jieli/common/bin/clang 存在（注意目录层次）
# 3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#   可以通过 ulimit -n 8096 来设置一个较大的值
# 支持的目标
# make ad24n_voice_enhanced
# make ad24n_voice_toy

.PHONY: all clean ad24n_voice_enhanced ad24n_voice_toy clean_ad24n_voice_enhanced clean_ad24n_voice_toy

all: ad24n_voice_enhanced ad24n_voice_toy
	@echo +ALL DONE

clean: clean_ad24n_voice_enhanced clean_ad24n_voice_toy
	@echo +CLEAN DONE

ad24n_voice_enhanced:
	$(MAKE) -C . -f Makefile.ad24n_voice_enhanced

clean_ad24n_voice_enhanced:
	$(MAKE) -C . -f Makefile.ad24n_voice_enhanced clean

ad24n_voice_toy:
	$(MAKE) -C . -f Makefile.ad24n_voice_toy

clean_ad24n_voice_toy:
	$(MAKE) -C . -f Makefile.ad24n_voice_toy clean
