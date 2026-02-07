#include "wdt_main.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include "cutils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLocale locale = QLocale::system();
    QString language = locale.name();//zh_CN
    QTranslator translator;
    if(!language.contains("CN")){
        if(translator.load(":/language_zh_EN.qm")){
            qApp->installTranslator(&translator);
        }
    }

    log_init(true);

    time_t tmp = 0;
    struct tm tm_ret;
    localtime_s(&tm_ret,&tmp); /* 此处调用是为了保证ex_time.c中调用时不出现未定义的错误提示，暂未找到问题原因?? */

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    CWdtMain w;
    w.show();

    return a.exec();
}
