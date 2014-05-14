# Simple tool to update binary and config file respectively
# 20140206 Cui.Yingyun

mm -j4
adb push $1/system/bin/RenderEvaluator /system/bin/
adb shell mkdir -p /data/RendererEvaluator
adb push Resource/config.ini /data/RendererEvaluator

echo ""
echo ""
echo "-Run RenderEvaluator-"
adb shell RenderEvaluator
