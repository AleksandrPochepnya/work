#ifndef EXCELBSSOURCETEMPLATEWIDGET_H
#define EXCELBSSOURCETEMPLATEWIDGET_H

#include <QWidget>
#include "Database/BaseStationsSourceTemplate.h"
#include <memory>
#include <QCheckBox>
#include <QComboBox>

class BaseStationsSourceTemplateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BaseStationsSourceTemplateWidget(const std::shared_ptr<BaseStationsSourceTemplate>& temp, QWidget *parent = nullptr);

private:
    QCheckBox* typeCheckBox = nullptr;
    QComboBox* typeColumnCheckBox = nullptr;

    std::shared_ptr<BaseStationsSourceTemplate> temp;

    void initLayout();
    QWidget* makeMappingBlockWidget();
    QWidget *makeBsTypesBlockWidget();
};

#endif // EXCELBSSOURCETEMPLATEWIDGET_H
