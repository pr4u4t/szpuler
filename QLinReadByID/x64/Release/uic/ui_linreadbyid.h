/********************************************************************************
** Form generated from reading UI file 'linreadbyid.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINREADBYID_H
#define UI_LINREADBYID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLinReadByIDUI
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QLabel *failedLabel;
    QLabel *progressLabel;
    QLabel *successLabel;
    QSpacerItem *horizontalSpacer_4;
    QFormLayout *formLayout_2;
    QLabel *label;
    QLineEdit *result;
    QLabel *label_2;
    QLabel *resultAscii;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *QLinReadByIDUI)
    {
        if (QLinReadByIDUI->objectName().isEmpty())
            QLinReadByIDUI->setObjectName("QLinReadByIDUI");
        QLinReadByIDUI->resize(400, 300);
        verticalLayout = new QVBoxLayout(QLinReadByIDUI);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(QLinReadByIDUI);
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(title);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        failedLabel = new QLabel(QLinReadByIDUI);
        failedLabel->setObjectName("failedLabel");
        failedLabel->setEnabled(false);
        failedLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        failedLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(failedLabel);

        progressLabel = new QLabel(QLinReadByIDUI);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setEnabled(false);
        progressLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        progressLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(progressLabel);

        successLabel = new QLabel(QLinReadByIDUI);
        successLabel->setObjectName("successLabel");
        successLabel->setEnabled(false);
        successLabel->setStyleSheet(QString::fromUtf8("font-weight:bold;"));
        successLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(successLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_3);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        label = new QLabel(QLinReadByIDUI);
        label->setObjectName("label");

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        result = new QLineEdit(QLinReadByIDUI);
        result->setObjectName("result");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, result);

        label_2 = new QLabel(QLinReadByIDUI);
        label_2->setObjectName("label_2");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        resultAscii = new QLabel(QLinReadByIDUI);
        resultAscii->setObjectName("resultAscii");

        formLayout_2->setWidget(1, QFormLayout::FieldRole, resultAscii);


        verticalLayout->addLayout(formLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(QLinReadByIDUI);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(QLinReadByIDUI);

        QMetaObject::connectSlotsByName(QLinReadByIDUI);
    } // setupUi

    void retranslateUi(QWidget *QLinReadByIDUI)
    {
        QLinReadByIDUI->setWindowTitle(QCoreApplication::translate("QLinReadByIDUI", "Form", nullptr));
        title->setText(QCoreApplication::translate("QLinReadByIDUI", "Read By ID", nullptr));
        failedLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "FAILED", nullptr));
        progressLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "IN PROGRESS", nullptr));
        successLabel->setText(QCoreApplication::translate("QLinReadByIDUI", "SUCCESS", nullptr));
        label->setText(QCoreApplication::translate("QLinReadByIDUI", "HEX:", nullptr));
        label_2->setText(QCoreApplication::translate("QLinReadByIDUI", "ASCII: ", nullptr));
        resultAscii->setText(QString());
        pushButton->setText(QCoreApplication::translate("QLinReadByIDUI", "Read", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLinReadByIDUI: public Ui_QLinReadByIDUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINREADBYID_H
