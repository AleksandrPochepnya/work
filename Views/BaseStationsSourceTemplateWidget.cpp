#include "BaseStationsSourceTemplateWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include "Database/Excel/ExcelBaseStationsSource.h"
#include <QDebug>
#include <QDebug>
#include <QGroupBox>
#include <QLineEdit>
#include <QDebug>
#include <QScrollArea>
#include <QScrollBar>
#include <QIntValidator>

BaseStationsSourceTemplateWidget::BaseStationsSourceTemplateWidget(const std::shared_ptr<BaseStationsSourceTemplate> &temp, QWidget *parent)
    : QWidget(parent), temp(temp)
{
    initLayout();
}

void BaseStationsSourceTemplateWidget::initLayout()
{
    auto rangeLayout = new QGridLayout();
    rangeLayout->setContentsMargins(0,5,0,5);

    auto firstRowLabel = new QLabel(tr("First row"), this);
    firstRowLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    rangeLayout->addWidget(firstRowLabel, 0, 0);
    QSpinBox * firstRowValue = new QSpinBox(this);
    firstRowValue->setRange(1, INT_MAX);
    firstRowValue->setValue(temp->firstRow);
    connect(firstRowValue, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=](int value) {
        temp->firstRow = value;
    });

    rangeLayout->addWidget(firstRowValue, 0, 1);

    QHBoxLayout* blocks = new QHBoxLayout();
    blocks->setContentsMargins(0,5,0,5);
    blocks->addWidget(makeMappingBlockWidget());

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(5,5,5,5);
    vLayout->addLayout(rangeLayout);
    vLayout->addWidget(makeBsTypesBlockWidget());

    blocks->addLayout(vLayout);
    blocks->addStretch();

    setLayout(blocks);
}

QWidget *BaseStationsSourceTemplateWidget::makeMappingBlockWidget()
{
    QGridLayout* mappingBlock = new QGridLayout();
    mappingBlock->setVerticalSpacing(3);
    mappingBlock->setHorizontalSpacing(3);
    mappingBlock->setContentsMargins(5,5,5,5);
    int row = 0;

    auto fieldsOrder = BaseStation::fieldsOrder();
    auto bsFields = BaseStation::fieldNames();
    for(auto field: fieldsOrder)
    {
        bool newField = !temp->columnsMapping.contains(field);
        ColumnMappingOptions& options = temp->columnsMapping[field];
        QComboBox* columnSelection = new QComboBox(this);
        columnSelection->setEnabled(false);

        auto columns = ExcelBaseStationsSource::columnNamesList();
        for(const auto& column: columns)
        {
            columnSelection->addItem(column);
        }
        int columnId = newField && field == BaseStation::Field::TYPE
                ? temp->columnsMapping[BaseStation::Field::NAME].column
                : options.column;

        columnSelection->setCurrentIndex(columnId - 1);
        connect(columnSelection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&options](int index) {
            options.column = index + 1;
        });

        QLineEdit* defaultEdit = nullptr;
        auto defaultValue = BaseStation::defaultFieldValues().value(field);
        if(!defaultValue.isEmpty())
        {
            defaultEdit = new QLineEdit(options.defaultValue, this);
            auto range = BaseStation::fieldValuesRanges().value(field);
            defaultEdit->setValidator(new QIntValidator(range.first, range.second, this));
            defaultEdit->setToolTip(QString(tr("Default value for %1")).arg(bsFields.value(field)));
            connect(defaultEdit, &QLineEdit::textChanged, this, [&options](const QString& text){
                options.defaultValue = text;
            });
        }

        QCheckBox* negativeCheckBox = nullptr;
        if(BaseStation::fieldCanBeNegative(field))
        {
            negativeCheckBox = new QCheckBox(tr("Negative"), this);
            negativeCheckBox->setChecked(options.negative);
            connect(negativeCheckBox, &QCheckBox::toggled, this, [&options](bool checked){
                options.negative = checked;
            });
        }

        QCheckBox* selectCheckBox = new QCheckBox(bsFields.value(field), this);
        QFont font;
        font.setWeight(QFont::Medium);
        selectCheckBox->setFont(font);

        connect(selectCheckBox, &QCheckBox::toggled, this, [&options, columnSelection, defaultEdit, negativeCheckBox](bool checked){
            options.select = checked;
            columnSelection->setEnabled(checked);
            if(defaultEdit != nullptr)
            {
                defaultEdit->setEnabled(!checked);
            }
            if(negativeCheckBox != nullptr)
            {
                negativeCheckBox->setVisible(checked);
            }
        });

        if(field == BaseStation::Field::TYPE)
        {
            typeCheckBox = selectCheckBox;
            typeCheckBox->setEnabled(false);
            typeColumnCheckBox = columnSelection;
        }

        selectCheckBox->setChecked(options.select);

        QString descrText = BaseStation::fieldDescriptions().value(field);
        if(!descrText.isEmpty())
        {
            auto descrLabel = new QLabel(descrText, this);
            descrLabel->setMargin(0);
            descrLabel->setStyleSheet("QLabel{ font-size: 6pt; color: gray }");
            mappingBlock->addWidget(descrLabel, row++, 0, 1, 3);
        }

        mappingBlock->addWidget(selectCheckBox, row, 0);
        mappingBlock->addWidget(new QLabel("->", this), row, 1);
        mappingBlock->addWidget(columnSelection, row, 2);
        row++;

        if(defaultEdit != nullptr)
        {
            mappingBlock->addWidget(defaultEdit, row++, 0, 1, 3);
        }

        if(negativeCheckBox != nullptr)
        {
            mappingBlock->addWidget(negativeCheckBox, row++, 0, 1, 3);
            mappingBlock->setAlignment(negativeCheckBox, Qt::AlignRight);
        }

        mappingBlock->setRowMinimumHeight(row++, 5);
    }

    mappingBlock->setRowStretch(row, 1);

    QWidget *scrollContent = new QWidget(this);
    scrollContent->setLayout(mappingBlock);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setStyleSheet("QScrollArea{border: 0px}");
    scroll->setContentsMargins(5,5,5,5);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scroll->verticalScrollBar()->setHidden(true);
    scroll->verticalScrollBar()->resize(0,0);
    scroll->setWidget(scrollContent);

    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(3,3,3,3);
    grid->addWidget(scroll, 0, 0);
    grid->setColumnStretch(0, 1);

    QGroupBox* g = new QGroupBox(tr("Columns mapping"), this);
    g->setLayout(grid);

    return g;
}

QWidget *BaseStationsSourceTemplateWidget::makeBsTypesBlockWidget()
{
    auto bsTypeNames = BaseStation::typeNames();

    QComboBox* defaultBsType = new QComboBox(this);
    QComboBox* singleBsType = new QComboBox(this);
    singleBsType->setEnabled(false);
    for(auto bsType: bsTypeNames.keys())
    {
        defaultBsType->addItem(bsTypeNames.value(bsType), static_cast<int>(bsType));
        singleBsType->addItem(bsTypeNames.value(bsType), static_cast<int>(bsType));
    }

    defaultBsType->setCurrentIndex(defaultBsType->findData(static_cast<int>(temp->defaultBsType)));
    singleBsType->setCurrentIndex(singleBsType->findData(static_cast<int>(temp->singleBsType)));

    connect(defaultBsType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        temp->defaultBsType = static_cast<BaseStation::Type>(defaultBsType->itemData(index).toInt());
    });

    connect(singleBsType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        temp->singleBsType = static_cast<BaseStation::Type>(singleBsType->itemData(index).toInt());
    });

    QLabel* note = new QLabel(tr("Check base station type in list and enter regular expression to identify it's type. "
                                 "Choose default type for base stations that have not been able to determine a type by any pattern"), this);
    note->setStyleSheet("QLabel{ border: 1px solid lightgray; padding: 5px; color: gray; font-size: 7pt }");
    note->setWordWrap(true);

    QGridLayout* typesLayout = new QGridLayout();
    typesLayout->setContentsMargins(3,3,3,3);

    int row = 0;
    typesLayout->setRowMinimumHeight(row++, 10);
    typesLayout->addWidget(note, row++, 0, 1, 2);

    typesLayout->addWidget(new QLabel(tr("Default type"), this), row, 0);
    typesLayout->addWidget(defaultBsType, row++, 1);
    typesLayout->setRowMinimumHeight(row++, 10);

    for(auto bsType: bsTypeNames.keys())
    {
        BsTemplateOptions& options =  temp->bsTypesTemplates[bsType];

        QLineEdit* patternEdit = new QLineEdit(options.pattern, this);
        patternEdit->setEnabled(false);

        connect(patternEdit, &QLineEdit::textChanged, this, [&options](const QString& text){
            options.pattern = text;
        });

        QCheckBox* selectCheckBox = new QCheckBox(bsTypeNames[bsType], this);
        connect(selectCheckBox, &QCheckBox::toggled, this, [&options, patternEdit](bool checked){
            options.select = checked;
            patternEdit->setEnabled(checked);           
        });

        selectCheckBox->setChecked(options.select);

        typesLayout->addWidget(selectCheckBox, row, 0);       
        typesLayout->addWidget(patternEdit, row, 1);
        row++;
    }
    typesLayout->setRowStretch(row, 1);

    QWidget* typesContainer = new QWidget(this);
    typesContainer->setLayout(typesLayout);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(5,5,5,5);

    QCheckBox* singleType = new QCheckBox(tr("List contains stations of one type"), this);
    connect(singleType, &QCheckBox::toggled, this, [=](bool checked){
        typesContainer->setEnabled(!checked);
        singleBsType->setEnabled(checked);
        temp->useSingleBsType = checked;

        typeColumnCheckBox->setEnabled(!checked);
        typeCheckBox->setChecked(!checked);
    });

    singleType->setChecked(temp->useSingleBsType);

    vLayout->addWidget(singleType);
    vLayout->addWidget(singleBsType);
    vLayout->addWidget(typesContainer);

    QWidget *scrollContent = new QWidget(this);
    scrollContent->setLayout(vLayout);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setStyleSheet("QScrollArea{border: 0px}");
    scroll->setContentsMargins(5,5,5,5);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scroll->verticalScrollBar()->setHidden(true);
    scroll->verticalScrollBar()->resize(0,0);
    scroll->setWidget(scrollContent);

    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(0,0,0,0);
    grid->addWidget(scroll, 0, 0);
    grid->setColumnStretch(0, 1);

    QGroupBox* g = new QGroupBox(tr("Base station types"), this);
    g->setLayout(grid);
    return g;
}
