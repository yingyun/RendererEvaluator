# Simple tool to update binary and config file respectively
# 20140206 Cui.Yingyun
# Note: Replace A_BUILD_RESULT as your android build directory

mm -j4
A_BUILD_RESULT=/home/yingyun/code/android-4.4_r1/out/target/product/flo
adb push $A_BUILD_RESULT/system/bin/RenderEvaluator /system/bin/
adb shell mkdir -p /data/RenderEvaluator
adb push Resource/config.ini /data/RenderEvaluator

echo "-Run RenderEvaluator-"
adb shell RenderEvaluator
