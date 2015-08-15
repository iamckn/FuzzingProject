/********************************************************************************
** Form generated from reading UI file 'aboutd.ui'
**
** Created: Fri Aug 15 21:49:06 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTD_H
#define UI_ABOUTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_aboutD
{
public:
    QWidget *horizontalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox2;
    QGridLayout *gridLayout;
    QTextEdit *texttupian;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout_2;
    QTextEdit *textjianjie;

    void setupUi(QDialog *aboutD)
    {
        if (aboutD->objectName().isEmpty())
            aboutD->setObjectName(QString::fromUtf8("aboutD"));
        aboutD->resize(555, 598);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(aboutD->sizePolicy().hasHeightForWidth());
        aboutD->setSizePolicy(sizePolicy);
        aboutD->setMaximumSize(QSize(1003, 600));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img1/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        aboutD->setWindowIcon(icon);
        horizontalLayoutWidget = new QWidget(aboutD);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 401, 581));
        verticalLayout = new QVBoxLayout(horizontalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        groupBox2 = new QGroupBox(horizontalLayoutWidget);
        groupBox2->setObjectName(QString::fromUtf8("groupBox2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(2);
        sizePolicy1.setHeightForWidth(groupBox2->sizePolicy().hasHeightForWidth());
        groupBox2->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        groupBox2->setFont(font);
        groupBox2->setFlat(false);
        groupBox2->setCheckable(false);
        gridLayout = new QGridLayout(groupBox2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        texttupian = new QTextEdit(groupBox2);
        texttupian->setObjectName(QString::fromUtf8("texttupian"));
        texttupian->setReadOnly(true);

        gridLayout->addWidget(texttupian, 1, 0, 1, 1);


        verticalLayout->addWidget(groupBox2);

        groupBox1 = new QGroupBox(horizontalLayoutWidget);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        groupBox1->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(groupBox1->sizePolicy().hasHeightForWidth());
        groupBox1->setSizePolicy(sizePolicy2);
        groupBox1->setFont(font);
        gridLayout_2 = new QGridLayout(groupBox1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        textjianjie = new QTextEdit(groupBox1);
        textjianjie->setObjectName(QString::fromUtf8("textjianjie"));
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        textjianjie->setFont(font1);
        textjianjie->setReadOnly(true);

        gridLayout_2->addWidget(textjianjie, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox1);


        retranslateUi(aboutD);

        QMetaObject::connectSlotsByName(aboutD);
    } // setupUi

    void retranslateUi(QDialog *aboutD)
    {
        aboutD->setWindowTitle(QApplication::translate("aboutD", "\347\263\273\347\273\237\347\256\200\344\273\213", 0, QApplication::UnicodeUTF8));
        groupBox2->setTitle(QApplication::translate("aboutD", "\347\263\273\347\273\237\346\213\223\346\211\221\345\233\276", 0, QApplication::UnicodeUTF8));
        texttupian->setHtml(QApplication::translate("aboutD", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'\345\276\256\350\275\257\351\233\205\351\273\221'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/img1/pic.png\" /></p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox1->setTitle(QApplication::translate("aboutD", "\347\263\273\347\273\237\347\256\200\344\273\213", 0, QApplication::UnicodeUTF8));
        textjianjie->setHtml(QApplication::translate("aboutD", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'\345\276\256\350\275\257\351\233\205\351\273\221'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'\345\256\213\344\275\223'; font-size:11pt;\">    \346\234\254\347\263\273\347\273\237\346\230\257\344\270\200\346\254\276\351\222\210\345\257\271\347\247\273\345\212\250\347\273\210\347\253\257\351\253\230\346\225\210\343\200\201\345\207\206\347\241\256\345\222\214\350\246\206\347\233\226\347\216\207\351\253\230\347\232\204\345\256\211\345\205\250\346\250\241\347\263\212\346\265\213\350\257\225\347\263\273\347\273\237\343\200\202\351\200\232\350\277\207\350\257\245\345\271\263\345\217\260\345\217"
                        "\257\345\234\250\346\227\240\347\272\277\351\223\276\350\267\257\344\276\247\345\220\221\347\247\273\345\212\250\347\273\210\347\253\257\345\217\221\351\200\201\347\225\270\345\275\242\347\232\204</span><span style=\" font-family:'Times New Roman,serif'; font-size:11pt;\">GSM</span><span style=\" font-family:'\345\256\213\344\275\223'; font-size:11pt;\">\346\227\240\347\272\277\344\277\241\344\273\244\346\225\260\346\215\256\345\216\273\346\214\226\346\216\230\345\217\257\350\203\275\345\255\230\345\234\250\347\232\204\345\237\272\345\270\246\346\274\217\346\264\236\343\200\202\347\263\273\347\273\237\345\274\200\345\217\221\345\214\205\346\213\254\347\241\254\344\273\266\345\271\263\345\217\260\345\217\212\347\233\270\345\205\263\350\275\257\344\273\266\347\232\204\345\256\236\347\216\260\357\274\214\345\256\214\346\210\220</span><span style=\" font-family:'Times New Roman,serif'; font-size:11pt;\">GSM</span><span style=\" font-family:'\345\256\213\344\275\223'; font-size:11pt;\">\346\227\240\347\272\277\351\223"
                        "\276\350\267\257\351\200\232\344\277\241\357\274\214</span><span style=\" font-family:'Times New Roman,serif'; font-size:11pt;\">GSM</span><span style=\" font-family:'\345\256\213\344\275\223'; font-size:11pt;\">\345\237\272\347\253\231\345\215\217\350\256\256\346\240\210\345\212\237\350\203\275\357\274\233\345\234\250\346\255\244\345\237\272\347\241\200\344\270\212\351\222\210\345\257\271\346\227\240\347\272\277\351\223\276\350\267\257</span><span style=\" font-family:'Times New Roman,serif'; font-size:11pt;\">GSM</span><span style=\" font-family:'\345\256\213\344\275\223'; font-size:11pt;\">\347\254\254\344\270\211\345\261\202\345\215\217\350\256\256\345\217\212\347\237\255\344\277\241\345\215\217\350\256\256\357\274\214\346\236\204\351\200\240\347\233\270\345\272\224\347\232\204\347\225\270\345\275\242\346\225\260\346\215\256\345\271\266\345\217\221\351\200\201\347\273\231\347\247\273\345\212\250\347\273\210\347\253\257\357\274\233\350\277\233\344\270\200\346\255\245\347\232\204\357\274\214\345\274\200\345\217"
                        "\221\344\270\200\346\254\276\345\272\224\347\224\250\350\275\257\344\273\266\345\256\236\346\227\266\347\233\221\346\265\213\347\247\273\345\212\250\347\273\210\347\253\257\345\237\272\345\270\246\345\244\204\347\220\206\345\231\250\347\232\204\350\277\220\350\241\214\346\203\205\345\206\265\357\274\214\346\234\200\347\273\210\345\256\214\346\210\220\345\256\211\345\205\250\346\250\241\347\263\212\346\265\213\350\257\225\345\271\266\344\270\224\347\273\231\345\207\272\346\265\213\350\257\225\346\212\245\345\221\212\343\200\202</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class aboutD: public Ui_aboutD {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTD_H
