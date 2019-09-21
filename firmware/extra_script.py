Import('env')
env.Replace(UPLOADCMD='$UPLOADER $UPLOADERFLAGS -e -U flash:w:$SOURCES:i')
