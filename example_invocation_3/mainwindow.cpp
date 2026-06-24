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


#include "Ztiny.h"


void buildSequenceItem      (QTreeWidget *parent, std::shared_ptr<zhy::tiny_program::sequence_tree> seq);
void buildSequenceItem      (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::sequence_tree> t);
void buildStatementItem     (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::statement_tree> t);
void buildIfItem            (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::if_tree> t);
void buildRepeatItem        (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::repeat_tree> t);
void buildAssignItem        (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::assign_tree> t);
void buildReadItem          (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::read_tree> t);
void buildWriteItem         (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::write_tree> t);
void buildForItem           (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::for_tree> t);
void buildWhileItem         (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::while_tree> t);
void buildUnaryItem         (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::unary_tree> t);
void buildUnaryStmtItem     (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::unary_statement_tree> t);

void buildExpressionItem    (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::expression_tree> t);
void buildSimpleExprItem    (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::simple_expression_tree> t);
void buildTermItem          (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::term_tree> t);
void buildExponentItem      (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::exponent_tree> t);
void buildFactorItem        (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::factor_tree> t);

void buildAssignRegexpItem  (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::assign_regexp_tree> t);
void buildRegexpItem        (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::regexp_tree> t);
void buildRegexpConnectItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::connect_exp_tree> t);
void buildRegexpUnaryItem   (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::regunary_exp_tree> t);
void buildRegexpAtomItem    (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::atom_tree> t);

void buildSequenceItem(QTreeWidget *parent, std::shared_ptr<zhy::tiny_program::sequence_tree> seq)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(seq->getName()));
    if (!seq) return;
    for (auto &stmt : seq->statements) {
        buildStatementItem(item, stmt);
    }
}

void buildSequenceItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::sequence_tree> seq)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(seq->getName()));
    if (!seq) return;
    for (auto &stmt : seq->statements) {
        buildStatementItem(item, stmt);
    }
}

void buildStatementItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::statement_tree> stmt)
{
    if (!stmt) return;

    if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::if_tree>(stmt))
        buildIfItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::repeat_tree>(stmt))
        buildRepeatItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::assign_tree>(stmt))
        buildAssignItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::read_tree>(stmt))
        buildReadItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::write_tree>(stmt))
        buildWriteItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::for_tree>(stmt))
        buildForItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::while_tree>(stmt))
        buildWhileItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::unary_statement_tree>(stmt))
        buildUnaryStmtItem(parent, t);
    else if (auto t = std::dynamic_pointer_cast<zhy::tiny_program::assign_regexp_tree>(stmt))
        buildAssignRegexpItem(parent, t);
}

void buildIfItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::if_tree> t)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->expression) buildExpressionItem(item, t->expression);
    if (t->then) buildSequenceItem(item, t->then);
    if (t->ifId && t->_else) buildSequenceItem(item, t->_else);
}

void buildRepeatItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::repeat_tree> t)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->repeat) buildSequenceItem(item, t->repeat);
    if (t->expression) buildExpressionItem(item, t->expression);
}

void buildAssignItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::assign_tree> t)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->expression) buildExpressionItem(item, t->expression);
}

void buildReadItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::read_tree> t)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
}

void buildWriteItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::write_tree> t)
{
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->expression) buildExpressionItem(item, t->expression);
}

void buildForItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::for_tree> t){
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->assign) buildAssignItem(item, t->assign);
    if (t->expression) buildExpressionItem(item, t->expression);
    if (t->unary) buildUnaryStmtItem(item, t->unary);
    if (t->repeat) buildSequenceItem(item, t->repeat);
}

void buildWhileItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::while_tree> t){
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->expression) buildExpressionItem(item, t->expression);
    if (t->repeat) buildSequenceItem(item, t->repeat);
}

void buildUnaryItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::unary_tree> t){
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
}

void buildUnaryStmtItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::unary_statement_tree> t){
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->unary) buildUnaryItem(item, t->unary);
}

void buildExpressionItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::expression_tree> t)
{
    //if (!t) return;
    if (t->id != 0) {
        if (t->left) buildSimpleExprItem(parent, t->left);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->left) buildSimpleExprItem(item, t->left);
    if (t->right) buildSimpleExprItem(item, t->right);
}

void buildSimpleExprItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::simple_expression_tree> t)
{
    //if (!t) return;
    if (t->id != 0) {
        if (t->term) buildTermItem(parent, t->term);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->simple) buildSimpleExprItem(item, t->simple);
    if (t->term) buildTermItem(item, t->term);
}

void buildTermItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::term_tree> t)
{
    //if (!t) return;
    if (t->id != 0) {
        if (t->exponent) buildExponentItem(parent, t->exponent);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->term) buildTermItem(item, t->term);
    if (t->exponent) buildExponentItem(item, t->exponent);
}

void buildExponentItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::exponent_tree> t)
{
    //if (!t) return;
    if (t->id != 0) {
        if (t->base) buildFactorItem(parent, t->base);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->base) buildFactorItem(item, t->base);
    if (t->exponent) buildExponentItem(item, t->exponent);
}

void buildFactorItem(QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::factor_tree> t)
{
    //if (!t) return;
    switch (t->id){
    case 0:if (t->expression) buildExpressionItem(parent, t->expression); return;
    case 1:{auto item = new QTreeWidgetItem(parent);item->setText(0, QString::fromStdString(t->getName())); return;}
    case 2:{auto item = new QTreeWidgetItem(parent);item->setText(0, QString::fromStdString(t->getName())); return;}
    case 3:if (t->unary) buildUnaryItem(parent, t->unary); return;
    }
}

void buildAssignRegexpItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::assign_regexp_tree> t){
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->regexp) buildRegexpItem(item, t->regexp);
}

void buildRegexpItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::regexp_tree> t){
    if (t->id != 0) {
        if (t->connect) buildRegexpConnectItem(parent, t->connect);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->regexp) buildRegexpItem(item, t->regexp);
    if (t->connect) buildRegexpConnectItem(item, t->connect);
}

void buildRegexpConnectItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::connect_exp_tree> t){
    if (t->id != 0) {
        if (t->regunary) buildRegexpUnaryItem(parent, t->regunary);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->connect) buildRegexpConnectItem(item, t->connect);
    if (t->regunary) buildRegexpUnaryItem(item, t->regunary);
}

void buildRegexpUnaryItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::regunary_exp_tree> t){
    if (t->id != 0) {
        if (t->atom) buildRegexpAtomItem(parent, t->atom);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
    if (t->atom) buildRegexpAtomItem(item, t->atom);
    auto newItem = new QTreeWidgetItem(item);
    newItem->setText(0, QString::fromStdString(t->unaryOperator));
}

void buildRegexpAtomItem (QTreeWidgetItem *parent, std::shared_ptr<zhy::tiny_program::atom_tree> t){
    if (t->id == 0){
        if (t->regexp) buildRegexpItem(parent, t->regexp);
        return;
    }
    auto item = new QTreeWidgetItem(parent);
    item->setText(0, QString::fromStdString(t->getName()));
}

void buildSyntaxTree(QTreeWidget *tree, std::shared_ptr<zhy::tiny_program::sequence_tree> root)
{
    if (!tree || !root) return;

    tree->clear();
    tree->setHeaderHidden(true);

    buildSequenceItem(tree, root);
    tree->expandAll();
}







MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("张淏禹tiny语言语法分析器");

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
    QPushButton *lexicalResultBtn = new QPushButton("词法分析结果");
    QPushButton *syntaxResultBtn = new QPushButton("语法分析结果");

    topBtnLayout->addWidget(openFileBtn);
    topBtnLayout->addWidget(saveFileBtn);
    topBtnLayout->addWidget(analysisBeginBtn);
    topBtnLayout->addStretch();
    topBtnLayout->addWidget(lexicalResultBtn);
    topBtnLayout->addWidget(syntaxResultBtn);
    pageLayout->addLayout(topBtnLayout);

    QHBoxLayout *mainContentLayout = new QHBoxLayout();

    QTextEdit *leftInput = new QTextEdit();
    leftInput->setPlaceholderText("请输入源程序或打开一个文件");

    QStackedWidget *rightStack = new QStackedWidget();

    QTableWidget *lexicalTable = new QTableWidget();
    lexicalTable->setColumnCount(5);
    lexicalTable->setHorizontalHeaderLabels({"序号", "tokenWord", "token类型", "行号", "列号"});
    lexicalTable->setWordWrap(true);
    lexicalTable->setTextElideMode(Qt::ElideNone);
    lexicalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QTreeWidget *syntaxTreeWidget = new QTreeWidget();
    syntaxTreeWidget->setHeaderHidden(true);
    rightStack->addWidget(syntaxTreeWidget);

    rightStack->addWidget(lexicalTable);
    rightStack->addWidget(syntaxTreeWidget);

    mainContentLayout->addWidget(leftInput, 3);
    mainContentLayout->addWidget(rightStack, 3);

    pageLayout->addLayout(mainContentLayout, 1);


    QHBoxLayout *bottomBtnLayout = new QHBoxLayout();
    QPushButton *btnLexError = new QPushButton("词法错误: 0 个");
    QPushButton *btnSyntaxError = new QPushButton("语法错误: 0 个");
    QPushButton *btnToggleError = new QPushButton("错误面板");
    bottomBtnLayout->addWidget(btnLexError);
    bottomBtnLayout->addWidget(btnSyntaxError);
    bottomBtnLayout->addWidget(btnToggleError);
    bottomBtnLayout->addStretch();
    pageLayout->addLayout(bottomBtnLayout);

    QStackedWidget *errorStack = new QStackedWidget();

    QTableWidget *lexErrorTable = new QTableWidget();
    lexErrorTable->setColumnCount(5);
    lexErrorTable->setHorizontalHeaderLabels({"序号", "词法错误描述", "文件", "行号", "列号"});
    lexErrorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    lexErrorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    errorStack->addWidget(lexErrorTable);

    QTableWidget *syntaxErrorTable = new QTableWidget();
    syntaxErrorTable->setColumnCount(5);
    syntaxErrorTable->setHorizontalHeaderLabels({"序号", "语法错误描述", "文件", "行号", "列号"});
    syntaxErrorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    syntaxErrorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    errorStack->addWidget(syntaxErrorTable);

    errorStack->setVisible(false);
    pageLayout->addWidget(errorStack, 1);


    std::shared_ptr <QString> filePath = std::make_shared <QString>();

    connect(openFileBtn, &QPushButton::clicked,this, [=](){

        *filePath = QFileDialog::getOpenFileName(
            this,
            "打开文本文件",
            QDir::homePath(),
            "文本文件 (*.tny);;所有文件 (*.*)"
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

        std::ostringstream displayOut;
        std::string fp = filePath->toStdString();
        zhy::tiny_program tempProgram(fp,displayOut);
        std::vector <std::vector <std::string>> tokensTable;
        std::ostringstream lexerErr;
        tempProgram.getAllTokensTable(tokensTable,lexerErr);
        tempProgram.start();
        std::vector <std::vector <std::string>> lexicalErrors;
        std::vector <std::vector <std::string>> syntaxErrors;
        tempProgram.getLexicalErrorsTable(lexicalErrors);
        tempProgram.getSyntaxErrorsTable(syntaxErrors);

        QString lexerErrStr = QString::fromStdString(lexerErr.str());
        if (!lexerErrStr.isEmpty()){
            QMessageBox::warning(this, "错误", lexerErrStr);
        }

        lexicalTable->clearContents();
        lexicalTable->setRowCount(tokensTable.size());

        for(size_t row = 0; row < tokensTable.size(); row++) {
            for (size_t col = 0; col < tokensTable[row].size(); col++) {
                QString qstr = QString::fromStdString(tokensTable[row][col]);
                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                lexicalTable->setItem(row, col, item);
            }
        }


        std::shared_ptr <zhy::tiny_program::sequence_tree> root = tempProgram.getRoot();
        buildSyntaxTree(syntaxTreeWidget, root);

        btnLexError->setText("词法错误: " + QString::number(lexicalErrors.size()) + " 个");
        lexErrorTable->clearContents();
        lexErrorTable->setRowCount(lexicalErrors.size());

        for(size_t row = 0; row < lexicalErrors.size(); row++) {
            for (size_t col = 0; col < lexicalErrors[row].size(); col++) {
                QString qstr = QString::fromStdString(lexicalErrors[row][col]);
                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                lexErrorTable->setItem(row, col, item);
            }
        }

        btnSyntaxError->setText("语法错误: " + QString::number(syntaxErrors.size()) + " 个");
        syntaxErrorTable->clearContents();
        syntaxErrorTable->setRowCount(syntaxErrors.size());

        for(size_t row = 0; row < syntaxErrors.size(); row++) {
            for (size_t col = 0; col < syntaxErrors[row].size(); col++) {
                QString qstr = QString::fromStdString(syntaxErrors[row][col]);
                QTableWidgetItem *item = new QTableWidgetItem(qstr);
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                syntaxErrorTable->setItem(row, col, item);
            }
        }


    });


    connect(lexicalResultBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(0);

    });

    connect(syntaxResultBtn, &QPushButton::clicked,this, [=](){
        rightStack->setCurrentIndex(1);

    });


    connect(btnLexError, &QPushButton::clicked, this, [=](){
        errorStack->setVisible(true);
        errorStack->setCurrentIndex(0);
    });


    connect(btnSyntaxError, &QPushButton::clicked, this, [=](){
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
