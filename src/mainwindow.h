
#include <QMainWindow>
#include <QLabel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

public slots:
    void update_position_label(const QPoint&,const QPoint&);

    void on_openfileAction_triggered();

private slots:
    void on_rectangleTool_triggered(bool checked);

private:
    Ui::MainWindow *ui;
    QLabel *status_view_cord = nullptr;
    QLabel *status_scene_cord=nullptr;
};

