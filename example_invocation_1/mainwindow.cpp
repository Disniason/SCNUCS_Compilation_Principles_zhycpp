#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <sstream>
#include <string>
#include <memory>

#include "Zrust.h"

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include <QTextEdit>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("张淏禹Rust语言编译器");

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

    QLabel *label = new QLabel(page);
    label->setStyleSheet("font-size: 24px; color: black;");
    label->setAlignment(Qt::AlignCenter);

    QPushButton *displayAllTokenBtn = new QPushButton("词法分析并打印token", page);
    displayAllTokenBtn->setFixedSize(200, 35);
    displayAllTokenBtn->setStyleSheet("font-size: 14px;");
    pageLayout->addWidget(displayAllTokenBtn, 0, Qt::AlignCenter);
    connect(displayAllTokenBtn, &QPushButton::clicked,this, [=](){
        QDialog dialog(this);
        dialog.setWindowTitle("词法分析并打印token");
        dialog.resize(600, 100);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        std::shared_ptr <QString> selectedPath = std::make_shared <QString>();
        QLabel *selectFileLabel = new QLabel("选中的源文件：", &dialog);
        selectFileLabel->setStyleSheet("font-size: 14px;");
        QLineEdit *selectFileEdit = new QLineEdit(&dialog);
        selectFileEdit->setReadOnly(true);
        selectFileEdit->setFixedHeight(30);
        selectFileEdit->setStyleSheet("font-size: 14px;");

        QPushButton *selectFileBtn = new QPushButton("选择源文件", page);
        selectFileBtn->setFixedSize(120, 35);
        selectFileBtn->setStyleSheet("font-size: 14px;");
        layout->addWidget(selectFileBtn, 0, Qt::AlignCenter);
        connect(selectFileBtn, &QPushButton::clicked,this, [=](){

            *selectedPath = QFileDialog::getOpenFileName(
                this,
                "选择文件",
                QDir::homePath(),
                "Rust源文件 (*.rs)"
            );

            if (selectedPath->isEmpty()) {
                QMessageBox::information(this, "提示", "未选择任何源文件");
                return;
            }

            selectFileEdit->setText(*selectedPath);

        });

        layout->addWidget(selectFileLabel);
        layout->addWidget(selectFileEdit);

        QPushButton *processFileBtn = new QPushButton("开始分析", page);
        processFileBtn->setFixedSize(120, 35);
        processFileBtn->setStyleSheet("font-size: 14px;");
        layout->addWidget(processFileBtn, 0, Qt::AlignCenter);
        connect(processFileBtn, &QPushButton::clicked,this, [=](){

            if (selectedPath->isEmpty()) {
                QMessageBox::information(this, "提示", "未选择任何源文件");
                return;
            }

            std::ostringstream outStream;
            std::ostringstream errStream;
            QMessageBox *msgBox = new QMessageBox(
                QMessageBox::Information,
                "提示：正在处理文件" + *selectedPath,
                "正在处理文件：" + *selectedPath,
                QMessageBox::NoButton,
                this
                );
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->show();
            bool succ = zhy::rust_lexer::processFileLine(selectedPath->toStdString(),outStream,errStream);
            msgBox->close();
            if (!succ) errStream<<"词法分析失败"<<std::endl;

            QString outText = QString::fromStdString(outStream.str());
            QString errText = QString::fromStdString(errStream.str());

            if (!errText.isEmpty()){
                QMessageBox::critical(this, "错误", errText);
            }

            QDialog tokenDialog(this);
            tokenDialog.setWindowTitle("该文件的全部token");
            tokenDialog.resize(600, 800);
            QVBoxLayout *tokenLayout = new QVBoxLayout(&tokenDialog);

            QTextEdit *tokenEdit = new QTextEdit(&tokenDialog);
            tokenEdit->setReadOnly(true);
            tokenEdit->setFixedHeight(800);
            tokenEdit->setStyleSheet("font-size: 14px;");

            tokenEdit->setText(outText);
            tokenLayout->addWidget(tokenEdit);
            tokenDialog.exec();
        });

        dialog.exec();
    });

    QPushButton *outputAllTokenBtn = new QPushButton("词法分析并输出token", page);
    outputAllTokenBtn->setFixedSize(200, 35);
    outputAllTokenBtn->setStyleSheet("font-size: 14px;");
    pageLayout->addWidget(outputAllTokenBtn, 0, Qt::AlignCenter);
    connect(outputAllTokenBtn, &QPushButton::clicked,this, [=](){
        QDialog dialog(this);
        dialog.setWindowTitle("词法分析并输出token");
        dialog.resize(600, 200);
        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        std::shared_ptr <QString> selectedPath = std::make_shared <QString>();
        QLabel *selectFileLabel = new QLabel("选中的源文件：", &dialog);
        selectFileLabel->setStyleSheet("font-size: 14px;");
        QLineEdit *selectFileEdit = new QLineEdit(&dialog);
        selectFileEdit->setReadOnly(true);
        selectFileEdit->setFixedHeight(30);
        selectFileEdit->setStyleSheet("font-size: 14px;");

        QPushButton *selectFileBtn = new QPushButton("选择源文件", page);
        selectFileBtn->setFixedSize(120, 35);
        selectFileBtn->setStyleSheet("font-size: 14px;");
        layout->addWidget(selectFileBtn, 0, Qt::AlignCenter);
        connect(selectFileBtn, &QPushButton::clicked,this, [=](){

            *selectedPath = QFileDialog::getOpenFileName(
                this,
                "选择文件",
                QDir::homePath(),
                "Rust源文件 (*.rs)"
                );

            if (selectedPath->isEmpty()) {
                QMessageBox::information(this, "提示", "未选择任何源文件");
                return;
            }

            selectFileEdit->setText(*selectedPath);

        });

        layout->addWidget(selectFileLabel);
        layout->addWidget(selectFileEdit);

        std::shared_ptr <QString> outputPath = std::make_shared <QString>();
        QLabel *outputFileLabel = new QLabel("选中的输出文件(输出后可见)：", &dialog);
        outputFileLabel->setStyleSheet("font-size: 14px;");
        QLineEdit *outputFileEdit = new QLineEdit(&dialog);
        outputFileEdit->setReadOnly(true);
        outputFileEdit->setFixedHeight(30);
        outputFileEdit->setStyleSheet("font-size: 14px;");

        QPushButton *outputFileBtn = new QPushButton("选择输出文件夹", page);
        outputFileBtn->setFixedSize(120, 35);
        outputFileBtn->setStyleSheet("font-size: 14px;");
        layout->addWidget(outputFileBtn, 0, Qt::AlignCenter);
        connect(outputFileBtn, &QPushButton::clicked,this, [=](){

            *outputPath = QFileDialog::getExistingDirectory(
                this,
                "选择文件夹",
                QDir::homePath()
                );

            if (outputPath->isEmpty()) {
                QMessageBox::information(this, "提示", "未选择任何输出文件夹");
                return;
            }

        });

        QLabel *nameLabel = new QLabel("请输入输出的文件名称(不需要输入后缀)：", &dialog);
        nameLabel->setStyleSheet("font-size: 14px;");
        QLineEdit *nameEdit = new QLineEdit(&dialog);
        nameEdit->setFixedHeight(30);
        nameEdit->setStyleSheet("font-size: 14px;");
        nameEdit->setText("untitled");
        layout->addWidget(nameLabel);
        layout->addWidget(nameEdit);

        QLabel *typeLabel = new QLabel("请选择文件类型：", &dialog);
        typeLabel->setStyleSheet("font-size: 14px;");
        QComboBox *typeCombo = new QComboBox(&dialog);
        typeCombo->setFixedHeight(30);
        typeCombo->setStyleSheet("font-size: 14px;");
        typeCombo->addItems({".txt", ".h", ".c", ".cpp", ".py", ".rs", ".csv", ".json"});
        layout->addWidget(typeLabel);
        layout->addWidget(typeCombo);

        layout->addWidget(outputFileLabel);
        layout->addWidget(outputFileEdit);

        QPushButton *processFileBtn = new QPushButton("开始分析并输出", page);
        processFileBtn->setFixedSize(120, 35);
        processFileBtn->setStyleSheet("font-size: 14px;");
        layout->addWidget(processFileBtn, 0, Qt::AlignCenter);
        connect(processFileBtn, &QPushButton::clicked,this, [=](){

            if (selectedPath->isEmpty()) {
                QMessageBox::information(this, "提示", "未选择任何源文件");
                return;
            }

            QString name = nameEdit->text().trimmed();
            QString type = typeCombo->currentText().trimmed();
            outputFileEdit->setText(*outputPath + "/" + name + type);

            if (outputPath->isEmpty() || name.isEmpty() || type.isEmpty()){
                QMessageBox::information(this, "提示", "输出文件路径存在问题");
                return;
            }

            std::ostringstream errStream;

            QMessageBox *msgBox = new QMessageBox(
                QMessageBox::Information,
                "提示：正在处理文件" + *selectedPath,
                "正在处理文件：" + *selectedPath,
                QMessageBox::NoButton,
                this
                );
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->show();
            bool succ = zhy::rust_lexer::outputFileProcessFileLine(selectedPath->toStdString(),outputPath->toStdString(),name.toStdString(),type.toStdString(),errStream);
            msgBox->close();
            if (!succ) errStream<<"词法分析失败"<<std::endl;

            QString errText = QString::fromStdString(errStream.str());
            if (!errText.isEmpty()){
                QMessageBox::critical(this, "错误", errText);
            }

        });

        dialog.exec();
    });

    return page;
}
