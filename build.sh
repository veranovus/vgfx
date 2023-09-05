PROJECT="cgame"
BINARY_DIR="Debug"
TARGET_DIR="target"
BUILD_DIR="build"

(cd $TARGET_DIR; mkdir .tmp/)

if [ -f "$TARGET_DIR/$BINARY_DIR/$PROJECT" ]
then
    (cd $TARGET_DIR; mv $BINARY_DIR/$PROJECT .tmp/) 
fi

cmake -S . -B $TARGET_DIR/$BUILD_DIR
cmake --build $TARGET_DIR/$BUILD_DIR

if [ -f "$TARGET_DIR/$BINARY_DIR/$PROJECT" ]
then
    (cd $TARGET_DIR; rm -fr .tmp/)
    ./$TARGET_DIR/$BINARY_DIR/$PROJECT
else
    (cd $TARGET_DIR; mv .tmp/$PROJECT $BINARY_DIR/; rmdir .tmp/)
fi
