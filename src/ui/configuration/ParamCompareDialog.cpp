#include "QsLog.h"
#include "configuration.h"
#include "ParamCompareDialog.h"
#include "ui_ParamCompareDialog.h"
#include <QMessageBox>
#include <QTableWidget>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QCheckBox>
#include <QPushButton>

#define PCD_COLUMN_PARAM_NAME 0
#define PCD_COLUMN_VALUE 1
#define PCD_COLUMN_NEW_VALUE 2
#define PCD_COLUMN_CHECKBOX 3

ParamCompareDialog::ParamCompareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParamCompareDialog),
    m_newList(new QMap<QString, UASParameter*>())
{
    ui->setupUi(this);

    QStringList headerList;
    headerList << tr("Parameter") << tr("Value") << tr("New Value") << tr("Use");

    QTableWidget* table = ui->compareTableWidget;
    table->setColumnCount(headerList.count());
    table->setHorizontalHeaderLabels(headerList);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setColumnWidth(PCD_COLUMN_CHECKBOX, 40);

    initConnections();
}

ParamCompareDialog::~ParamCompareDialog()
{
    delete ui;
}

void ParamCompareDialog::initConnections()
{
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadParameterFile()));
    connect(ui->continueButton, SIGNAL(clicked()), this, SLOT(saveNewParameters()));
    connect(ui->checkAllBox, SIGNAL(clicked()), this, SLOT(checkAll()));
}

void ParamCompareDialog::loadParameterFile()
{
    ui->compareTableWidget->setRowCount(0);

    QString homeDir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    QDir parameterDir = QDir(homeDir + APP_DATA_DIRECTORY + PARAMETER_DIRECTORY);

    if(!parameterDir.exists())
        parameterDir.mkdir(parameterDir.path());

    QString filename = QFileDialog::getOpenFileName(this,tr("Open File To Compare"),
                                                    parameterDir.path(), "*.param");

    if(filename.length() == 0) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"Error","Unable to open the file.");
        return;
    }

    QString filestring = file.readAll();
    file.close();

    populateParamListFromString(filestring, m_newList);

    compareLists();
}

void ParamCompareDialog::setCurrentList(QMap<QString, UASParameter *> &aParamaterList)
{
    m_currentList = &aParamaterList;
}

void ParamCompareDialog::populateParamListFromString(QString paramString, QMap<QString, UASParameter*>* list)
{
    QStringList paramSplit = paramString.split("\n");

    foreach (QString paramLine, paramSplit) {
        if (!paramLine.startsWith("#")) {
            QStringList lineSplit = paramLine.split(",");
            if (lineSplit.size() == 2)
            {
                bool ok;
                QLOG_DEBUG() << "load param: " << lineSplit[0] << "=" << lineSplit[1];
                UASParameter* param = new UASParameter();
                param->setName(lineSplit[0]);

                double value = lineSplit[1].toFloat(&ok);
                if (ok){
                    param->setValue(QVariant(value));
                } else {
                    QLOG_ERROR() << "Conversion Failure";
                    param->setValue(QVariant("NaN"));
                }

                list->insert(param->name(), param);

            }
        } else {
            QLOG_DEBUG() << "Comment: " << paramLine;
        }
    }
}

void ParamCompareDialog::compareLists()
{
    QList<QString> keys = m_newList->keys(); // This needs to be an amalgamated list of all keys

    ui->compareTableWidget->setSortingEnabled(false);
    for(int count = 0; count < keys.count(); ++count){

        UASParameter* currentParam = m_currentList->value(keys[count]);

        if (currentParam != NULL){
            UASParameter* newParam = m_newList->value(keys[count]);

            if (currentParam->value() != newParam->value() ){
                QLOG_DEBUG() << "Difference : " << currentParam->name()
                             << " current: " << currentParam->value() << " new:" << newParam->value();

                int rowCount = ui->compareTableWidget->rowCount();
                ui->compareTableWidget->setRowCount(ui->compareTableWidget->rowCount()+1);

                QTableWidgetItem* widgetItemParam = new QTableWidgetItem(keys[count]);
                widgetItemParam->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled );
                ui->compareTableWidget->setItem(rowCount, PCD_COLUMN_PARAM_NAME, widgetItemParam);

                QTableWidgetItem* widgetItemValue = new QTableWidgetItem();
                widgetItemValue->setData(Qt::DisplayRole, currentParam->value());
                widgetItemValue->setFlags(Qt::NoItemFlags  | Qt::ItemIsEnabled);
                ui->compareTableWidget->setItem(rowCount, PCD_COLUMN_VALUE, widgetItemValue);

                QTableWidgetItem* widgetItemNewValue = new QTableWidgetItem();
                widgetItemNewValue->setData(Qt::DisplayRole, newParam->value());
                widgetItemNewValue->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsEditable);
                ui->compareTableWidget->setItem(rowCount, PCD_COLUMN_NEW_VALUE, widgetItemNewValue);

                QTableWidgetItem* widgetItemCheckbox= new QTableWidgetItem();
                widgetItemCheckbox->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                widgetItemCheckbox->setCheckState(Qt::Checked);
                ui->compareTableWidget->setItem(rowCount, PCD_COLUMN_CHECKBOX, widgetItemCheckbox);

            }
        } else {
            // [TODO] add in a blanck entry?
        }
    }
}

void ParamCompareDialog::saveNewParameters()
{
    QLOG_DEBUG() << " Save selected Parameters";
    QTableWidget* table = ui->compareTableWidget;

    for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
        // Get hold of the UASParameter
        QTableWidgetItem* paramName = table->item(rowCount, PCD_COLUMN_PARAM_NAME);
        QTableWidgetItem* paramCheck= table->item(rowCount, PCD_COLUMN_CHECKBOX);
        if (paramName && (paramCheck->checkState() == Qt::Checked)){
            UASParameter* param = m_currentList->value(paramName->text());

            // then update it's value in the current list
            // [TODO] this would be an action of the Param Manager
            param->setValue(table->item(rowCount, PCD_COLUMN_NEW_VALUE)->data(Qt::DisplayRole));
            QLOG_DEBUG() << "Applied to m_currentList:" << param << " = "
                         << table->item(rowCount, PCD_COLUMN_NEW_VALUE)->data(Qt::DisplayRole);
        }
    }

    accept(); // dismiss the dialog
}

void ParamCompareDialog::checkAll()
{
    QLOG_DEBUG() << " check uncheck all parameters";
    QTableWidget* table = ui->compareTableWidget;

    if(ui->checkAllBox->isChecked()){
        for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
            QTableWidgetItem* item = table->item(rowCount, PCD_COLUMN_CHECKBOX);
            if (item) item->setCheckState(Qt::Checked);
        }
    } else {
        for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
            QTableWidgetItem* item = table->item(rowCount, PCD_COLUMN_CHECKBOX);
            if (item) item->setCheckState(Qt::Unchecked);
        }
    }
}
