#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "Zregexp.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("张淏禹词法分析程序生成器");

    this->stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(this->stackedWidget);
    this->mainface = createPage(Qt::white);
    this->stackedWidget->addWidget(mainface);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearLayout(QLayout *layout){
    if (!layout) return;

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr){
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        else if (item->layout()) {
            clearLayout(item->layout());
        }
        delete item;
    }
}


QWidget* MainWindow::createPage(const QColor &bgColor) {
    QWidget *page = new QWidget(stackedWidget);
    page->setStyleSheet(QString("background-color: %1;").arg(bgColor.name()));

    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(10, 10, 10, 10);
    pageLayout->setSpacing(8);

    QHBoxLayout *topBtnLayout = new QHBoxLayout();
    QPushButton *openFileBtn = new QPushButton("打开文件");
    QPushButton *saveFileBtn = new QPushButton("保存到文件");
    QPushButton *analysisBeginBtn = new QPushButton("开始分析");
    QPushButton *displayLexerBtn = new QPushButton("显示词法分析源程序");
    QPushButton *outputFileBtn = new QPushButton("输出到文件");

    topBtnLayout->addWidget(openFileBtn);
    topBtnLayout->addWidget(saveFileBtn);
    topBtnLayout->addWidget(analysisBeginBtn);
    topBtnLayout->addStretch();
    topBtnLayout->addWidget(displayLexerBtn);
    topBtnLayout->addWidget(outputFileBtn);

    pageLayout->addLayout(topBtnLayout);
    QHBoxLayout *mainContentLayout = new QHBoxLayout();

    QTextEdit *leftInput = new QTextEdit();
    leftInput->setPlaceholderText("请输入正则表达式或打开一个文件");

    std::shared_ptr <QString> filePath = std::make_shared <QString>();

    connect(openFileBtn, &QPushButton::clicked,this, [=](){

        *filePath = QFileDialog::getOpenFileName(
            this,
            "打开文本文件",
            QDir::homePath(),
            "文本文件 (*.txt);;所有文件 (*.*)"
            );

        if (filePath->isEmpty()) {
            QMessageBox::information(this, "提示", "未选择任何文件");
            return;
        }

        QFile file(*filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
            return;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString fileContent = in.readAll();
        file.close();

        leftInput->setText(fileContent);
    });

    connect(saveFileBtn, &QPushButton::clicked,this, [=](){

        QString fileContent = leftInput->toPlainText();

        QFile file(*filePath);

        if (filePath->isEmpty()){
            *filePath = QFileDialog::getSaveFileName(
                this,
                "保存正则表达式",
                QDir::homePath() + "/正则表达式.txt",
                "文本文件 (*.txt);;所有文件 (*)"
                );
        }

        if (filePath->isEmpty()){
            QMessageBox::information(this, "提示", "未选择任何文件");
            return;
        }

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法打开文件进行写入！");
            return;
        }

        QTextStream out(&file);
        out << fileContent;

        file.close();

        QMessageBox::information(this, "成功", "文件已保存到：\n" + *filePath);
    });

    QVBoxLayout *middleBtnLayout = new QVBoxLayout();
    std::shared_ptr <zhy::regexp_nfa_dfas <zhy::nfa_graph,zhy::nfa_state_transition_table>> rndl = std::make_shared <zhy::regexp_nfa_dfas <zhy::nfa_graph,zhy::nfa_state_transition_table>>();


    connect(analysisBeginBtn, &QPushButton::clicked,this, [=](){
        clearLayout(middleBtnLayout);
        QString regexpText = leftInput->toPlainText();
        std::istringstream iss(regexpText.toStdString());
        std::ostringstream out;
        std::ostringstream err;

        zhy::regexp_nfa_dfa_lexer_generator::getRegexpsNfasDfasLexers <
            zhy::nfa_graph,
            zhy::nfa_state_transition_table
        >(iss,*rndl,out,err);

        QString errText = QString::fromStdString(err.str());
        if (!errText.isEmpty()){
            QMessageBox::critical(this, "错误", errText);
        }

        const std::vector <zhy::regexp_nfa_dfa <zhy::nfa_graph,zhy::nfa_state_transition_table>> & tables = rndl->tables;
        for(const zhy::regexp_nfa_dfa <zhy::nfa_graph,zhy::nfa_state_transition_table> & rnd : tables){
            QString name = QString::fromStdString(rnd.getRegexpName());
            QPushButton *mBtn = new QPushButton(name);

            connect(mBtn, &QPushButton::clicked,this, [=](){
                QDialog dialog(this);
                dialog.setWindowTitle("正则表达式" + name + "的分析结果");
                dialog.resize(400, 300);
                QVBoxLayout *layout = new QVBoxLayout(&dialog);

                QPushButton *lexerResultBtn = new QPushButton("查看词法分析结果", page);
                layout->addWidget(lexerResultBtn, 0, Qt::AlignCenter);
                connect(lexerResultBtn, &QPushButton::clicked,this, [=](){
                    QDialog lexerResultDialog(this);
                    lexerResultDialog.setWindowTitle("词法分析结果");
                    lexerResultDialog.resize(600, 800);
                    QVBoxLayout *lexerResultLayout = new QVBoxLayout(&lexerResultDialog);

                    QTextEdit *lexerResultEdit = new QTextEdit(&lexerResultDialog);
                    lexerResultEdit->setReadOnly(true);
                    lexerResultEdit->setFixedHeight(800);
                    lexerResultEdit->setStyleSheet("font-size: 14px;");

                    QString lexerResultText = QString::fromStdString(rnd.regexpTokensStr);
                    lexerResultEdit->setText(lexerResultText);
                    lexerResultLayout->addWidget(lexerResultEdit);
                    lexerResultDialog.exec();

                });

                if (rnd.needDfa()){

                    QPushButton *nfaTableBtn = new QPushButton("查看NFA图状态转换表", page);
                    layout->addWidget(nfaTableBtn, 0, Qt::AlignCenter);
                    connect(nfaTableBtn, &QPushButton::clicked,this, [=](){
                        QDialog nfaTableDialog(this);
                        nfaTableDialog.setWindowTitle("NFA图状态转换表");
                        nfaTableDialog.resize(800, 600);
                        QVBoxLayout *nfaTableLayout = new QVBoxLayout(&nfaTableDialog);

                        std::vector <std::vector <std::string>> value;
                        rnd.nfa.getValue(value);

                        QTableWidget* nfaTable = new QTableWidget(this);
                        nfaTable->setRowCount(value.size());
                        nfaTable->setColumnCount(value[0].size());

                        for(size_t row = 0; row < value.size(); row++) {
                            for (size_t col = 0; col < value[row].size(); col++) {
                                QString qstr = QString::fromStdString(value[row][col]);
                                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                                nfaTable->setItem(row, col, item);
                            }
                        }

                        nfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                        nfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                        nfaTable->resizeRowsToContents();

                        nfaTableLayout->addWidget(nfaTable);
                        nfaTableDialog.exec();

                    });

                    QPushButton *dfaTableBtn = new QPushButton("查看DFA图状态转换表", page);
                    layout->addWidget(dfaTableBtn, 0, Qt::AlignCenter);
                    connect(dfaTableBtn, &QPushButton::clicked,this, [=](){
                        QDialog dfaTableDialog(this);
                        dfaTableDialog.setWindowTitle("DFA图状态转换表");
                        dfaTableDialog.resize(800, 600);
                        QVBoxLayout *dfaTableLayout = new QVBoxLayout(&dfaTableDialog);

                        std::vector <std::vector <std::string>> value;
                        rnd.dfa.getValue(value);

                        QTableWidget* dfaTable = new QTableWidget(this);
                        dfaTable->setRowCount(value.size());
                        dfaTable->setColumnCount(value[0].size());

                        for(size_t row = 0; row < value.size(); row++) {
                            for (size_t col = 0; col < value[row].size(); col++) {
                                QString qstr = QString::fromStdString(value[row][col]);
                                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                                dfaTable->setItem(row, col, item);
                            }
                        }

                        dfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                        dfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                        dfaTable->resizeRowsToContents();


                        dfaTableLayout->addWidget(dfaTable);
                        dfaTableDialog.exec();

                    });

                    QPushButton *minDfaTableBtn = new QPushButton("查看最小化DFA图状态转换表", page);
                    layout->addWidget(minDfaTableBtn, 0, Qt::AlignCenter);
                    connect(minDfaTableBtn, &QPushButton::clicked,this, [=](){
                        QDialog minDfaTableDialog(this);
                        minDfaTableDialog.setWindowTitle("最小化DFA图状态转换表");
                        minDfaTableDialog.resize(800, 600);
                        QVBoxLayout *minDfaTableLayout = new QVBoxLayout(&minDfaTableDialog);

                        std::vector <std::vector <std::string>> value;
                        rnd.minDfa.getValue(value);

                        QTableWidget* minDfaTable = new QTableWidget(this);
                        minDfaTable->setRowCount(value.size());
                        minDfaTable->setColumnCount(value[0].size());

                        for(size_t row = 0; row < value.size(); row++) {
                            for (size_t col = 0; col < value[row].size(); col++) {
                                QString qstr = QString::fromStdString(value[row][col]);
                                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                                minDfaTable->setItem(row, col, item);
                            }
                        }

                        minDfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                        minDfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                        minDfaTable->resizeRowsToContents();

                        minDfaTableLayout->addWidget(minDfaTable);
                        minDfaTableDialog.exec();

                    });


                }

                dialog.exec();
            });

            middleBtnLayout->addWidget(mBtn);
        }

        QString name = QString::fromStdString(rndl->getLexerName());
        QPushButton *totalBtn = new QPushButton(name);

        connect(totalBtn, &QPushButton::clicked,this, [=](){
            QDialog dialog(this);
            dialog.setWindowTitle("正则表达式" + name + "的分析结果");
            dialog.resize(400, 300);
            QVBoxLayout *layout = new QVBoxLayout(&dialog);

            QPushButton *nfaTableBtn = new QPushButton("查看NFA图状态转换表", page);
            layout->addWidget(nfaTableBtn, 0, Qt::AlignCenter);
            connect(nfaTableBtn, &QPushButton::clicked,this, [=](){
                QDialog nfaTableDialog(this);
                nfaTableDialog.setWindowTitle("NFA图状态转换表");
                nfaTableDialog.resize(800, 600);
                QVBoxLayout *nfaTableLayout = new QVBoxLayout(&nfaTableDialog);

                std::vector <std::vector <std::string>> value;
                rndl->total.nfa.getValue(value);

                QTableWidget* nfaTable = new QTableWidget(this);
                nfaTable->setRowCount(value.size());
                nfaTable->setColumnCount(value[0].size());

                for(size_t row = 0; row < value.size(); row++) {
                    for (size_t col = 0; col < value[row].size(); col++) {
                        QString qstr = QString::fromStdString(value[row][col]);
                        QTableWidgetItem *item = new QTableWidgetItem(qstr);
                        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                        nfaTable->setItem(row, col, item);
                    }
                }

                nfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                nfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                nfaTable->resizeRowsToContents();

                nfaTableLayout->addWidget(nfaTable);
                nfaTableDialog.exec();

            });

            QPushButton *dfaTableBtn = new QPushButton("查看DFA图状态转换表", page);
            layout->addWidget(dfaTableBtn, 0, Qt::AlignCenter);
            connect(dfaTableBtn, &QPushButton::clicked,this, [=](){
                QDialog dfaTableDialog(this);
                dfaTableDialog.setWindowTitle("DFA图状态转换表");
                dfaTableDialog.resize(800, 600);
                QVBoxLayout *dfaTableLayout = new QVBoxLayout(&dfaTableDialog);

                std::vector <std::vector <std::string>> value;
                rndl->total.dfa.getValue(value);

                QTableWidget* dfaTable = new QTableWidget(this);
                dfaTable->setRowCount(value.size());
                dfaTable->setColumnCount(value[0].size());

                for(size_t row = 0; row < value.size(); row++) {
                    for (size_t col = 0; col < value[row].size(); col++) {
                        QString qstr = QString::fromStdString(value[row][col]);
                        QTableWidgetItem *item = new QTableWidgetItem(qstr);
                        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                        dfaTable->setItem(row, col, item);
                    }
                }

                dfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                dfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                dfaTable->resizeRowsToContents();


                dfaTableLayout->addWidget(dfaTable);
                dfaTableDialog.exec();

            });

            QPushButton *minDfaTableBtn = new QPushButton("查看最小化DFA图状态转换表", page);
            layout->addWidget(minDfaTableBtn, 0, Qt::AlignCenter);
            connect(minDfaTableBtn, &QPushButton::clicked,this, [=](){
                QDialog minDfaTableDialog(this);
                minDfaTableDialog.setWindowTitle("最小化DFA图状态转换表");
                minDfaTableDialog.resize(800, 600);
                QVBoxLayout *minDfaTableLayout = new QVBoxLayout(&minDfaTableDialog);

                std::vector <std::vector <std::string>> value;
                rndl->total.minDfa.getValue(value);

                QTableWidget* minDfaTable = new QTableWidget(this);
                minDfaTable->setRowCount(value.size());
                minDfaTable->setColumnCount(value[0].size());

                for(size_t row = 0; row < value.size(); row++) {
                    for (size_t col = 0; col < value[row].size(); col++) {
                        QString qstr = QString::fromStdString(value[row][col]);
                        QTableWidgetItem *item = new QTableWidgetItem(qstr);
                        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                        minDfaTable->setItem(row, col, item);
                    }
                }

                minDfaTable->setStyleSheet("QTableWidget::item { padding: 5px; }");
                minDfaTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                minDfaTable->resizeRowsToContents();

                minDfaTableLayout->addWidget(minDfaTable);
                minDfaTableDialog.exec();

            });

            dialog.exec();
        });

        middleBtnLayout->addWidget(totalBtn);

        middleBtnLayout->addStretch();

    });

    QTextEdit *rightOutput = new QTextEdit();
    rightOutput->setPlaceholderText("词法分析源程序显示区(内容仅供参考)");
    rightOutput->setReadOnly(true);

    connect(displayLexerBtn, &QPushButton::clicked,this, [=](){

        QString lexerCode = QString::fromStdString(rndl->getLexerProgram());

        rightOutput->setText(lexerCode);
    });

    connect(outputFileBtn, &QPushButton::clicked,this, [=](){

        QString lexerCode = QString::fromStdString(rndl->getLexerProgram());

        QString savePath = QFileDialog::getSaveFileName(
            this,
            "保存词法分析程序",
            QDir::homePath() + "/lexer.cpp",
            "cpp源文件 (*.cpp);;所有文件 (*)"
            );

        if (savePath.isEmpty()) {
            QMessageBox::information(this, "提示", "未选择任何文件");
            return;
        }

        QFile file(savePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法打开文件进行写入！");
            return;
        }

        QTextStream out(&file);
        out << lexerCode;

        file.close();

        QMessageBox::information(this, "成功", "文件已保存到：\n" + savePath);
    });

    mainContentLayout->addWidget(leftInput, 3);
    mainContentLayout->addLayout(middleBtnLayout, 0);
    mainContentLayout->addWidget(rightOutput, 3);

    pageLayout->addLayout(mainContentLayout, 1);
    return page;
}
