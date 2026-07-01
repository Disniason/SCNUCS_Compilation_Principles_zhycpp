#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include <QLabel>
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
#include <QStackedWidget>
#include <QTreeWidget>


#include "Zlalr.h"






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("张淏禹LALR(1)语法分析器生成器");

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
    QPushButton *displayAllTokensBtn = new QPushButton("全部token");
    QPushButton *firstSetBtn = new QPushButton("first集合");
    QPushButton *followSetBtn = new QPushButton("follow集合");
    QPushButton *LR0DFAGraphBtn = new QPushButton("LR(0)DFA图");
    QPushButton *isSLR1GrammarBtn = new QPushButton("判断SLR(1)文法");
    QPushButton *LR1DFAGraphBtn = new QPushButton("LR(1)DFA图");
    QPushButton *LALR1DFAGraphBtn = new QPushButton("LALR(1)DFA图");

    topBtnLayout->addWidget(openFileBtn);
    topBtnLayout->addWidget(saveFileBtn);
    topBtnLayout->addWidget(analysisBeginBtn);

    topBtnLayout->addStretch();

    topBtnLayout->addWidget(displayAllTokensBtn);
    topBtnLayout->addWidget(firstSetBtn);
    topBtnLayout->addWidget(followSetBtn);
    topBtnLayout->addWidget(LR0DFAGraphBtn);
    topBtnLayout->addWidget(isSLR1GrammarBtn);
    topBtnLayout->addWidget(LR1DFAGraphBtn);
    topBtnLayout->addWidget(LALR1DFAGraphBtn);
    pageLayout->addLayout(topBtnLayout);

    QHBoxLayout *mainContentLayout = new QHBoxLayout();

    QTextEdit *leftInput = new QTextEdit();
    leftInput->setPlaceholderText("请输入源程序或打开一个文件");

    QStackedWidget *rightStack = new QStackedWidget();

    QTextEdit *allTokens = new QTextEdit();
    allTokens->setPlaceholderText("所有token将显示在这里");
    allTokens->setReadOnly(true);

    QTableWidget *firstSetTable = new QTableWidget();
    firstSetTable->setColumnCount(2);
    firstSetTable->setHorizontalHeaderLabels({"非终结符", "first集合"});
    firstSetTable->setWordWrap(true);
    firstSetTable->setTextElideMode(Qt::ElideNone);
    firstSetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QTableWidget *followSetTable = new QTableWidget();
    followSetTable->setColumnCount(2);
    followSetTable->setHorizontalHeaderLabels({"非终结符", "follow集合"});
    followSetTable->setWordWrap(true);
    followSetTable->setTextElideMode(Qt::ElideNone);
    followSetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QTableWidget *LR0DFAGraphTable = new QTableWidget();
    LR0DFAGraphTable->setWordWrap(true);
    LR0DFAGraphTable->setTextElideMode(Qt::ElideNone);
    LR0DFAGraphTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QTextEdit *isSLR1Grammar = new QTextEdit();
    isSLR1Grammar->setPlaceholderText("这里将显示SLR(1)文法判断结果");
    isSLR1Grammar->setReadOnly(true);

    QTableWidget *LR1DFAGraphTable = new QTableWidget();
    LR1DFAGraphTable->setWordWrap(true);
    LR1DFAGraphTable->setTextElideMode(Qt::ElideNone);
    LR1DFAGraphTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QTableWidget *LALR1DFAGraphTable = new QTableWidget();
    LALR1DFAGraphTable->setWordWrap(true);
    LALR1DFAGraphTable->setTextElideMode(Qt::ElideNone);
    LALR1DFAGraphTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rightStack->addWidget(allTokens);
    rightStack->addWidget(firstSetTable);
    rightStack->addWidget(followSetTable);
    rightStack->addWidget(LR0DFAGraphTable);
    rightStack->addWidget(isSLR1Grammar);
    rightStack->addWidget(LR1DFAGraphTable);
    rightStack->addWidget(LALR1DFAGraphTable);

    mainContentLayout->addWidget(leftInput, 3);
    mainContentLayout->addWidget(rightStack, 3);

    pageLayout->addLayout(mainContentLayout, 1);

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

        if (filePath->isEmpty()){
            *filePath = QFileDialog::getSaveFileName(
                this,
                "保存正则表达式",
                QDir::homePath() + "/untitled.tny",
                "文本文件 (*.tny);;所有文件 (*)"
                );
        }

        if (filePath->isEmpty()){
            QMessageBox::information(this, "提示", "未选择任何文件");
            return;
        }

        QFile file(*filePath);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法打开文件进行写入！");
            return;
        }

        QTextStream out(&file);
        out << fileContent;

        file.close();

        QMessageBox::information(this, "成功", "文件已保存到：\n" + *filePath);
    });


    connect(analysisBeginBtn, &QPushButton::clicked,this, [=](){
        if (filePath->isEmpty()){
            QMessageBox::information(this, "提示", "未选择任何文件");
            return;
        }

        std::ostringstream openFileError;
        zhy::grammar_all_work work(filePath->toStdString(),openFileError);
        QString openFileErr = QString::fromStdString(openFileError.str());
        if (!openFileErr.isEmpty()){
            QMessageBox::warning(this, "错误", openFileErr);
            return;
        }

        allTokens->clear();
        firstSetTable->clearContents();
        firstSetTable->setRowCount(0);
        followSetTable->clearContents();
        followSetTable->setRowCount(0);
        LR0DFAGraphTable->clearContents();
        LR0DFAGraphTable->setRowCount(0);
        isSLR1Grammar->clear();
        LR1DFAGraphTable->clearContents();
        LR1DFAGraphTable->setRowCount(0);
        LALR1DFAGraphTable->clearContents();
        LALR1DFAGraphTable->setRowCount(0);

        std::ostringstream allTokensGetter;
        std::ostringstream allTokensGetterError;
        work.getAllTokens(allTokensGetter,allTokensGetterError);
        QString allTokensGetterErr = QString::fromStdString(openFileError.str());
        if (!allTokensGetterErr.isEmpty()){
            QMessageBox::warning(this, "错误", allTokensGetterErr);
        }
        else{
            QString allTokensText = QString::fromStdString(allTokensGetter.str());
            allTokens->setText(allTokensText);
        }

        std::vector <std::vector <std::string>> firstSetTableContent;
        std::ostringstream firstSetError;
        work.getFirstSetTable(firstSetTableContent,firstSetError);
        QString firstSetErr = QString::fromStdString(firstSetError.str());
        if (!firstSetErr.isEmpty()){
            QMessageBox::warning(this, "错误", firstSetErr);
        }
        else{
            firstSetTable->setColumnCount(firstSetTableContent[0].size());
            firstSetTable->setRowCount(firstSetTableContent.size());
            for(size_t row = 0; row < firstSetTableContent.size(); row++) {
                for (size_t col = 0; col < firstSetTableContent[row].size(); col++) {
                    QString qstr = QString::fromStdString(firstSetTableContent[row][col]);
                    QTableWidgetItem *item = new QTableWidgetItem(qstr);
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    firstSetTable->setItem(row, col, item);
                }
            }
        }

        std::vector <std::vector <std::string>> followSetTableContent;
        std::ostringstream followSetError;
        work.getFollowSetTable(followSetTableContent,followSetError);
        QString followSetErr = QString::fromStdString(followSetError.str());
        if (!followSetErr.isEmpty()){
            QMessageBox::warning(this, "错误", followSetErr);
        }
        else{
            followSetTable->setColumnCount(followSetTableContent[0].size());
            followSetTable->setRowCount(followSetTableContent.size());
            for(size_t row = 0; row < followSetTableContent.size(); row++) {
                for (size_t col = 0; col < followSetTableContent[row].size(); col++) {
                    QString qstr = QString::fromStdString(followSetTableContent[row][col]);
                    QTableWidgetItem *item = new QTableWidgetItem(qstr);
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    followSetTable->setItem(row, col, item);
                }
            }
        }

        std::vector<std::vector<std::string>> LR0TableContent;
        std::ostringstream LR0Error;
        work.getLR0Table(LR0TableContent, LR0Error);
        QString LR0Err = QString::fromStdString(LR0Error.str());
        if (!LR0Err.isEmpty()){
            QMessageBox::warning(this, "错误", LR0Err);
        }
        else{
            LR0DFAGraphTable->setColumnCount(LR0TableContent[0].size());
            LR0DFAGraphTable->setRowCount(LR0TableContent.size());
            for (size_t row = 0; row < LR0TableContent.size(); row++) {
                for (size_t col = 0; col < LR0TableContent[row].size(); col++) {
                    QString qstr = QString::fromStdString(LR0TableContent[row][col]);
                    QTableWidgetItem *item = new QTableWidgetItem(qstr);
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    LR0DFAGraphTable->setItem(static_cast<int>(row), static_cast<int>(col), item);
                }
            }
        }

        std::ostringstream slr1Desc;
        std::ostringstream slr1Err;
        bool slr1Result = work.isSLR1(slr1Desc, slr1Err);

        QString slr1ErrorStr = QString::fromStdString(slr1Err.str());
        if (!slr1ErrorStr.isEmpty()) {
            QMessageBox::warning(this, "错误", slr1ErrorStr);
        }
        else{
            if (slr1Result){
                isSLR1Grammar->setText("是SLR(1)文法");
            }
            else{
                isSLR1Grammar->setText("不是SLR(1)文法，原因如下：");
                QString resultText = QString::fromStdString(slr1Desc.str());
                isSLR1Grammar->append(resultText);
            }
        }

        std::vector<std::vector<std::string>> LR1TableContent;
        std::ostringstream LR1Error;
        work.getLR1Table(LR1TableContent, LR1Error);
        QString LR1Err = QString::fromStdString(LR1Error.str());
        if (!LR1Err.isEmpty()){
            QMessageBox::warning(this, "错误", LR1Err);
        }
        else{
            LR1DFAGraphTable->setColumnCount(LR1TableContent[0].size());
            LR1DFAGraphTable->setRowCount(LR1TableContent.size());
            for (size_t row = 0; row < LR1TableContent.size(); row++) {
                for (size_t col = 0; col < LR1TableContent[row].size(); col++) {
                    QString qstr = QString::fromStdString(LR1TableContent[row][col]);
                    QTableWidgetItem *item = new QTableWidgetItem(qstr);
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    LR1DFAGraphTable->setItem(static_cast<int>(row), static_cast<int>(col), item);
                }
            }
        }

        std::vector<std::vector<std::string>> LALR1TableContent;
        std::ostringstream LALR1Error;
        work.getLALR0Table(LALR1TableContent, LALR1Error);
        QString LALR1Err = QString::fromStdString(LALR1Error.str());
        if (!LALR1Err.isEmpty()){
            QMessageBox::warning(this, "错误", LALR1Err);
        }
        else{
            LALR1DFAGraphTable->setColumnCount(LALR1TableContent[0].size());
            LALR1DFAGraphTable->setRowCount(LALR1TableContent.size());
            for (size_t row = 0; row < LALR1TableContent.size(); row++) {
                for (size_t col = 0; col < LALR1TableContent[row].size(); col++) {
                    QString qstr = QString::fromStdString(LALR1TableContent[row][col]);
                    QTableWidgetItem *item = new QTableWidgetItem(qstr);
                    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    LALR1DFAGraphTable->setItem(static_cast<int>(row), static_cast<int>(col), item);
                }
            }
        }


    });


    connect(displayAllTokensBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(0);

    });

    connect(firstSetBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(1);

    });

    connect(followSetBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(2);

    });

    connect(LR0DFAGraphBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(3);

    });

    connect(isSLR1GrammarBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(4);

    });

    connect(LR1DFAGraphBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(5);

    });

    connect(LALR1DFAGraphBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(6);

    });

    pageLayout->addLayout(mainContentLayout, 1);
    return page;
}
