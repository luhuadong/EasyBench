#include "eb_widget_util.h"

#include <QComboBox>
#include <QFormLayout>

namespace EbWidget {

void applyFormLayoutStyle(QFormLayout *form)
{
    if (!form) {
        return;
    }
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // 每行：左侧标签与右侧控件在垂直方向居中对齐（须在 addRow 完成之后调用）
    for (int row = 0; row < form->rowCount(); ++row) {
        if (QLayoutItem *labelItem = form->itemAt(row, QFormLayout::LabelRole)) {
            labelItem->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        if (QLayoutItem *fieldItem = form->itemAt(row, QFormLayout::FieldRole)) {
            // 仅垂直居中；水平拉伸由控件的 Expanding 策略负责
            fieldItem->setAlignment(Qt::AlignVCenter);
        }
    }
}

void applyComboBoxStyle(QComboBox *box)
{
    if (!box) {
        return;
    }

    box->setFixedHeight(32);
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx)
{
    if (!root || maxPopupHeightPx <= 0) {
        return;
    }

    const int itemHeight = 32;
    const int maxItems = qMax(6, maxPopupHeightPx / itemHeight);

    const QList<QComboBox *> boxes = root->findChildren<QComboBox *>();
    for (QComboBox *box : boxes) {
        box->setMaxVisibleItems(maxItems);
        applyComboBoxStyle(box);
    }
}

} // namespace EbWidget
