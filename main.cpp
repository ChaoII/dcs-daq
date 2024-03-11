#include "src/mainwindow.h"
#include <QApplication>

QString loadStyleSheet(const QString &styleSheetFile) {  //styleSheetFile是qss文件的Path
    QFile file(styleSheetFile);
    if (file.open(QFile::ReadOnly)) {
        //如果在MainWindow使用了setStyleSheet，对qss文件的内容进行追加
        //QString styleSheet = this->styleSheet();
        //styleSheet += QLatin1String(file.readAll());//读取样式表文件
        QString styleSheet = QLatin1String(file.readAll());  //QByteArray可直接转换为QString
        file.close();
        return styleSheet;
    } else {
        return "";
    }
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto style_sheet = loadStyleSheet(":/style.css");
    a.setStyleSheet(style_sheet);
    MainWindow w;
    w.show();
    return a.exec();
}
