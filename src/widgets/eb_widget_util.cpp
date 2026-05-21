#include "eb_widget_util.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <QSpinBox>

namespace EbWidget {

QLabel *createFormLabel(QWidget *parent, const QString &text)
{
    QLabel *label = new QLabel(text, parent);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return label;
}

void applyFormLayoutStyle(QFormLayout *form)
{
    if (!form) {
        return;
    }
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::DontWrapRows);

    for (int row = 0; row < form->rowCount(); ++row) {
        if (QLayoutItem *labelItem = form->itemAt(row, QFormLayout::LabelRole)) {
            labelItem->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        if (QLayoutItem *fieldItem = form->itemAt(row, QFormLayout::FieldRole)) {
            fieldItem->setAlignment(Qt::AlignVCenter);
        }
    }
}

void applyAllFormLayouts(QWidget *root)
{
    if (!root) {
        return;
    }
    const QList<QFormLayout *> forms = root->findChildren<QFormLayout *>();
    for (QFormLayout *form : forms) {
        applyFormLayoutStyle(form);
    }
}

void applyComboBoxStyle(QComboBox *box)
{
    if (!box) {
        return;
    }

    box->setFixedHeight(kFormFieldHeight);
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx)
{
    if (!root || maxPopupHeightPx <= 0) {
        return;
    }

    const int maxItems = qMax(6, maxPopupHeightPx / kFormFieldHeight);

    const QList<QComboBox *> boxes = root->findChildren<QComboBox *>();
    for (QComboBox *box : boxes) {
        box->setMaxVisibleItems(maxItems);
        applyComboBoxStyle(box);
    }
}

void applyLineEditStyle(QLineEdit *edit)
{
    if (!edit) {
        return;
    }
    edit->setFixedHeight(kFormFieldHeight);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applySpinBoxStyle(QSpinBox *spin)
{
    if (!spin) {
        return;
    }
    spin->setFixedHeight(kFormFieldHeight);
    spin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applyFormFieldStyles(QWidget *root)
{
    if (!root) {
        return;
    }
    const QList<QLineEdit *> edits = root->findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        applyLineEditStyle(edit);
    }
    const QList<QSpinBox *> spins = root->findChildren<QSpinBox *>();
    for (QSpinBox *spin : spins) {
        applySpinBoxStyle(spin);
    }
}

} // namespace EbWidget
