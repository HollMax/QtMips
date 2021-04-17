#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include "machine/machineconfig.h"
#include "ui_NewDialog.h"
#include "ui_NewDialogCache.h"

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

class NewDialogCacheHandler;

class NewDialog : public QDialog {
    Q_OBJECT
public:
    NewDialog(QWidget *parent, QSettings *settings);
    ~NewDialog() override;

    void switch2custom();

protected:
    void closeEvent(QCloseEvent *) override;

private slots:
    void cancel();
    void create();
    void create_empty();
    void browse_elf();
    void elf_change(QString val);
    void set_preset();
    void pipelined_change(bool);
    void delay_slot_change(bool);
    void hazard_unit_change();
    void mem_protec_exec_change(bool);
    void mem_protec_write_change(bool);
    void mem_time_read_change(int);
    void mem_time_write_change(int);
    void mem_time_burst_change(int);
    void osemu_enable_change(bool);
    void osemu_known_syscall_stop_change(bool);
    void osemu_unknown_syscall_stop_change(bool);
    void osemu_interrupt_stop_change(bool);
    void osemu_exception_stop_change(bool);
    void browse_osemu_fs_root();
    void osemu_fs_root_change(QString val);
    void reset_at_compile_change(bool);

private:
    Ui::NewDialog *ui {};
    Ui::NewDialogCache *ui_cache_p {}, *ui_cache_d {};
    QSettings *settings;

    machine::MachineConfig *config;
    void config_gui(); // Apply configuration to gui

    unsigned preset_number();
    void load_settings();
    void store_settings();
    NewDialogCacheHandler *cache_handler_p {}, *cache_handler_d {};
};

class NewDialogCacheHandler : public QObject {
    Q_OBJECT
public:
    NewDialogCacheHandler(NewDialog *nd, Ui::NewDialogCache *ui);

    void set_config(machine::CacheConfig *config);

    void config_gui();

private slots:
    void enabled(bool);
    void numsets();
    void blocksize();
    void degreeassociativity();
    void replacement(int);
    void writeback(int);

private:
    NewDialog *nd;
    Ui::NewDialogCache *ui {};
    machine::CacheConfig *config;
};

#endif // NEWDIALOG_H
