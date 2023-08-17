# 部署到公共库里面，根据自己的名字再公共库创建如下分支
# LIB_NAME  (默认就是TARGET)
#	Linux
#		include
#		lib		
#	Windows	
#		include
#		lib		
#

# 初始化目标文件名字
DEPLOY_TARGET=$${TARGET_DIR}
isEmpty(LIB_NAME):{
LIB_NAME=$${DEPLOY_TARGET}
}

# 确定平台名字
isEmpty(PROPALTFORM){
unix{
    PROPALTFORM=Linux
    CC_PARTS = $$split(QMAKE_CC,-)
    PROPALTFORM = ArmLinux
}
win32{
    PROPALTFORM=Windows
}
}

# 判断路径
isEmpty(PTVAR_EXTLIB):error(PTVAR_EXTLIB is empty)
isEmpty(PTVAR_LIB):error(PTVAR_LIB is empty)
isEmpty(PTVAR_INC):error(PTVAR_INC is empty)
isEmpty(PTVAR_BIN):error(PTVAR_BIN is empty)

#公共库库路径
MY_PUBLIB_DIR = $$PTVAR_EXTLIB/$${LIB_NAME}/$${PROPALTFORM}/lib$$ARCHSUFFIX
#公共库头文件路径
MY_PUBINC_DIR = $$PTVAR_EXTLIB/$${LIB_NAME}/$${PROPALTFORM}/include
win32{
MY_PUBLIB_DIR = $$replace(MY_PUBLIB_DIR,/,\\)
MY_PUBINC_DIR = $$replace(MY_PUBINC_DIR,/,\\)
}

message(deploy lib to public library path $$PTVAR_EXTLIB/$${LIB_NAME})

!isEmpty(QMAKE_POST_LINK):QMAKE_POST_LINK += $$escape_expand(\\n\\t)
unix{
    # 创建目录
    QMAKE_POST_LINK += mkdir -p $$MY_PUBLIB_DIR$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += mkdir -p $$MY_PUBINC_DIR$$escape_expand(\\n\\t)
    # 拷贝
    QMAKE_POST_LINK += cp -rf $$PTVAR_INC/* $$MY_PUBINC_DIR/$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += cp -rf $$PTVAR_BIN/lib$${TARGET}*.so* $$MY_PUBLIB_DIR$$escape_expand(\\n\\t)
}

win32{
    # 创建目录
    QMAKE_POST_LINK += if not exist $$escape_expand(\")$$MY_PUBLIB_DIR$$escape_expand(\") mkdir $$escape_expand(\")$$MY_PUBLIB_DIR$$escape_expand(\")$$escape_expand(\\n\\t)
    QMAKE_POST_LINK += if not exist $$escape_expand(\")$$MY_PUBINC_DIR$$escape_expand(\") mkdir $$escape_expand(\")$$MY_PUBINC_DIR$$escape_expand(\")$$escape_expand(\\n\\t)
    # 拷贝
    MY_PTVLIB_PATH = $$PTVAR_LIB/$${TARGET}*.
    MY_PTVLIB_PATH = $$replace(MY_PTVLIB_PATH,/,\\)
    MY_PTVINC_PATH = $$replace(PTVAR_INC,/,\\)
    QMAKE_POST_LINK += xcopy  $$escape_expand(\")$$MY_PTVINC_PATH$$escape_expand(\") $$escape_expand(\")$$MY_PUBINC_DIR$$escape_expand(\") /S /E /Y /I $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += xcopy  $$escape_expand(\")$${MY_PTVLIB_PATH}dll$$escape_expand(\") $$escape_expand(\")$$MY_PUBLIB_DIR$$escape_expand(\") /I /Y $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += xcopy  $$escape_expand(\")$${MY_PTVLIB_PATH}lib$$escape_expand(\") $$escape_expand(\")$$MY_PUBLIB_DIR$$escape_expand(\") /I /Y $$escape_expand(\\n\\t)
}

#message(=========================)
#message(QMAKE_POST_LINK = $$QMAKE_POST_LINK)
#message(=========================)
