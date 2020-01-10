isEmpty(LIB_NAME){
    error( "copy_file.pri err: Couldn't set the value LIB_NAME !" )
}
# Добавляем модуль file_copies
CONFIG += file_copies
# Добавляем переменную, описывающую копируемые файлы
COPIES += headers
# Определяем, с каким расширением нам необохдимо скопировать файлы и по какому пути они расположены
headers.files = $$files($${PWD}/src/$${LIB_NAME}/*.h)
# Указываем путь, куда копировать файлы
headers.path = $${LIBS_PATH}/$${LIB_NAME}/include

#message(Path: $${headers.path})

##****************** copy 2 ВО ВРЕМЯ ЗАПУСКА APP КОМИТИТЬ

#headers.path = $${LIBS_PATH}/dfs_base/include
#headers.files   += $$files($${PWD}/*.h)
#INSTALLS       += headers
