# Simple tool to update binary and config file respectively
# 20140206 Cui.Yingyun

mm -j4
adb remount
adb push $1/system/bin/RendererEvaluator /system/bin/
adb shell mkdir -p /data/RendererEvaluator
adb push Resource/config.ini /data/RendererEvaluator

echo ""
echo ""
echo "-Run RenderEvaluator-"
adb shell RendererEvaluator
