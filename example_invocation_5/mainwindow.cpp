#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <sstream>
#include <memory>

#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "Ztiny_tuple.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("张淏禹tiny扩充语言中间代码生成器");

    this->stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(this->stackedWidget);
    this->mainface = createPage(Qt::white);
    this->stackedWidget->addWidget(mainface);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget* MainWindow::createPage(const QColor &bgColor) {
    QWidget *page = new QWidget(stackedWidget);
    page->setStyleSheet(QString("background-color: %1;").arg(bgColor.name()));

    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(10, 10, 10, 10);
    pageLayout->setSpacing(8);

    //顶部所有按钮
    QHBoxLayout *topBtnLayout = new QHBoxLayout();
    QPushButton *openFileBtn = new QPushButton("打开文件");
    QPushButton *saveFileBtn = new QPushButton("保存到文件");
    QPushButton *analysisBeginBtn = new QPushButton("开始分析");
    QPushButton *displayGrammarRuleBtn = new QPushButton("查看文法规则");
    QPushButton *displayAllTokensBtn = new QPushButton("全部token");
    QPushButton *displayAllQuadrupleBtn = new QPushButton("显示四元组");
    QPushButton *saveQuadrupleBtn = new QPushButton("允许保存四元组到文件");

    topBtnLayout->addWidget(openFileBtn);
    topBtnLayout->addWidget(saveFileBtn);
    topBtnLayout->addWidget(analysisBeginBtn);
    topBtnLayout->addStretch();
    topBtnLayout->addWidget(displayGrammarRuleBtn);
    topBtnLayout->addWidget(displayAllTokensBtn);
    topBtnLayout->addWidget(displayAllQuadrupleBtn);
    topBtnLayout->addWidget(saveQuadrupleBtn);
    pageLayout->addLayout(topBtnLayout);

    //左侧输入框
    QHBoxLayout *mainContentLayout = new QHBoxLayout();
    QTextEdit *leftInput = new QTextEdit();
    leftInput->setPlaceholderText("请输入源程序或打开一个文件");

    //右侧显示区域
    QStackedWidget *rightStack = new QStackedWidget();
    QTextEdit *grammarRule = new QTextEdit();
    grammarRule->setText(QString::fromStdString(zhy::tiny_quadruple_getter::getGrammarRule()));
    grammarRule->setReadOnly(true);
    QTextEdit *allTokens = new QTextEdit();
    allTokens->setPlaceholderText("所有token将显示在这里");
    allTokens->setReadOnly(true);
    QTextEdit *AllQuadruples = new QTextEdit();
    AllQuadruples->setPlaceholderText("所有的四元组将显示在这里");
    AllQuadruples->setReadOnly(true);
    rightStack->addWidget(allTokens);
    rightStack->addWidget(AllQuadruples);
    rightStack->addWidget(grammarRule);

    mainContentLayout->addWidget(leftInput, 3);
    mainContentLayout->addWidget(rightStack, 3);
    pageLayout->addLayout(mainContentLayout, 1);

    //底部错误显示按钮
    QHBoxLayout *bottomBtnLayout = new QHBoxLayout();
    QPushButton *btnDisplayError = new QPushButton("语法错误: 0 个");
    QPushButton *btnDisplayWarning = new QPushButton("警告: 0 个");
    QPushButton *btnToggleError = new QPushButton("错误面板");
    bottomBtnLayout->addWidget(btnDisplayError);
    bottomBtnLayout->addWidget(btnDisplayWarning);
    bottomBtnLayout->addWidget(btnToggleError);
    bottomBtnLayout->addStretch();
    pageLayout->addLayout(bottomBtnLayout);

    //底部错误显示区
    QStackedWidget *errorStack = new QStackedWidget();
    QTextEdit *AllErrors = new QTextEdit();
    AllErrors->setPlaceholderText("所有的错误将显示在这里");
    AllErrors->setReadOnly(true);
    errorStack->addWidget(AllErrors);

    QTextEdit *AllWarnings = new QTextEdit();
    AllWarnings->setPlaceholderText("所有的警告将显示在这里");
    AllWarnings->setReadOnly(true);
    errorStack->addWidget(AllWarnings);

    errorStack->setVisible(false);
    pageLayout->addWidget(errorStack, 1);

    //业务处理代码
    std::shared_ptr <QString> filePath = std::make_shared <QString>();
    std::shared_ptr <bool> saveQuadruple = std::make_shared <bool>(true);

    if (*saveQuadruple){
        saveQuadrupleBtn->setText("允许保存四元组到文件");
    }
    else{
        saveQuadrupleBtn->setText("不允许保存四元组到文件");
    }

    connect(openFileBtn, &QPushButton::clicked,this, [=](){

        *filePath = QFileDialog::getOpenFileName(
            this,
            "打开tiny源文件",
            QDir::homePath(),
            "源文件 (*.tny);;所有文件 (*.*)"
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
                "保存tiny源文件",
                QDir::homePath() + "/untitled.tny",
                "源文件 (*.tny);;所有文件 (*)"
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

        std::ostringstream errStream;
        zhy::tiny_quadruple_getter quadruple(filePath->toStdString(),errStream);
        QString errStr = QString::fromStdString(errStream.str());
        if (!errStr.isEmpty()){
            QMessageBox::warning(this, "错误", errStr);
        }

        std::ostringstream tokensStream;
        quadruple.displayAllTokens(tokensStream);
        allTokens->setText(QString::fromStdString(tokensStream.str()));

        quadruple.start();
        std::ostringstream quadrupleStream;
        quadruple.display(quadrupleStream,true,true);
        AllQuadruples->setText(QString::fromStdString(quadrupleStream.str()));

        btnDisplayError->setText("语法错误: " + QString::number(quadruple.getErrorNum()) + " 个");
        std::ostringstream errorStream;
        quadruple.displayErrors(errorStream);
        AllErrors->setText(QString::fromStdString(errorStream.str()));

        btnDisplayWarning->setText("警告: " + QString::number(quadruple.getWarningNum()) + " 个");
        std::ostringstream warningStream;
        quadruple.displayWarnings(warningStream);
        AllWarnings->setText(QString::fromStdString(warningStream.str()));

        if (*saveQuadruple){
            std::shared_ptr <QString> outputPath = std::make_shared <QString>();
            *outputPath = QFileDialog::getSaveFileName(
                this,
                "保存四元组中间代码",
                QDir::homePath() + "/untitled.txt",
                "文本文件 (*.txt);;所有文件 (*)"
                );

            if (outputPath->isEmpty()){
                QMessageBox::information(this, "提示", "未选择任何文件");
                return;
            }

            std::ostringstream outputErrStream;
            quadruple.outputFile(outputPath->toStdString(),outputErrStream);
            QString outputErrStr = QString::fromStdString(outputErrStream.str());
            if (!outputErrStr.isEmpty()){
                QMessageBox::warning(this, "错误", outputErrStr);
            }
            else{
                QMessageBox::information(this, "成功", "文件已保存到：\n" + *outputPath);
            }
        }

        errStr = QString::fromStdString(errStream.str());
        if (!errStr.isEmpty()){
            QMessageBox::warning(this, "错误", errStr);
        }

    });

    connect(displayGrammarRuleBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(2);

    });

    connect(displayAllTokensBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(0);

    });

    connect(displayAllQuadrupleBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(1);

    });

    connect(saveQuadrupleBtn, &QPushButton::clicked, this, [=](){
        if (*saveQuadruple){
            saveQuadrupleBtn->setText("不允许保存四元组到文件");
            *saveQuadruple = false;
        }
        else{
            saveQuadrupleBtn->setText("允许保存四元组到文件");
            *saveQuadruple = true;
        }
    });

    connect(btnDisplayError, &QPushButton::clicked, this, [=](){
        errorStack->setVisible(true);
        errorStack->setCurrentIndex(0);
    });


    connect(btnDisplayWarning, &QPushButton::clicked, this, [=](){
        errorStack->setVisible(true);
        errorStack->setCurrentIndex(1);
    });


    connect(btnToggleError, &QPushButton::clicked, this, [=](){
        bool visible = errorStack->isVisible();
        errorStack->setVisible(!visible);

    });

    pageLayout->addLayout(mainContentLayout, 1);
    return page;
}
