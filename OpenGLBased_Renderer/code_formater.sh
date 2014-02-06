~/bin/astyle --style=gnu --indent=spaces --indent-switches --indent-labels --indent-preprocessor --indent-col1-comments  --recursive "./*.cpp"
~/bin/astyle --style=gnu --indent=spaces --indent-switches --indent-labels --indent-preprocessor --indent-col1-comments  --recursive "./*.c"
~/bin/astyle --style=gnu --indent=spaces --indent-switches --indent-labels --indent-preprocessor --indent-col1-comments  --recursive "./*.h"
find ./ -name "*.orig"  | xargs rm
chmod -R 777 ./*

