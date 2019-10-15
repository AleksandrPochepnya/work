#ifndef CONVERTINGERRORSDIALOG_H
#define CONVERTINGERRORSDIALOG_H

#include <QDialog>
#include <vector>
#include "Converting/AbstractBaseStationsConverter.h"

class ConvertingErrorsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConvertingErrorsDialog(const AbstractBaseStationsConverter* converter, QWidget *parent = nullptr);
};

#endif // CONVERTINGERRORSDIALOG_H
