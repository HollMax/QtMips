#ifndef SAVECHANGED_H
#define SAVECHANGED_H

#include <QDialog>
#include <QList>
#include <QStandardItemModel>
#include <QStringList>

class SaveChnagedDialog : public QDialog {
    Q_OBJECT

public:
    explicit SaveChnagedDialog(
        QStringList &changedlist,
        QWidget *parent = nullptr);
    ~SaveChnagedDialog() override;
signals:
    void user_decision(bool cancel, QStringList tosavelist);
private slots:
    void cancel_clicked();
    void ignore_clicked();
    void save_clicked();

private:
    QStandardItemModel *model;
};

#endif // SAVECHANGED_H
