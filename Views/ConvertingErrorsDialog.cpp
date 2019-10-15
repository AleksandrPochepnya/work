#include "ConvertingErrorsDialog.h"
#include <QListView>
#include <QTextEdit>
#include <QVBoxLayout>

ConvertingErrorsDialog::ConvertingErrorsDialog(const AbstractBaseStationsConverter *converter, QWidget *parent)
    : QDialog(parent)
{
    auto textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);

    auto f = textEdit->font();
    f.setPointSize(7);
    textEdit->setFont(f);

    for(const auto& row: converter->invalidRowsData())
    {
        textEdit->append(row);
    }

    auto vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->addWidget(textEdit);
    setLayout(vLayout);

    setMinimumSize(800, 500);

    setWindowTitle(tr("Invalid rows"));
}
