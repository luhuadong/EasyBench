#ifndef EB_WIDGET_UTIL_H
#define EB_WIDGET_UTIL_H

#include <QString>

class QComboBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QSpinBox;
class QWidget;

namespace EbWidget {

constexpr int kFormFieldHeight = 32;

QLabel *createFormLabel(QWidget *parent, const QString &text);

// 须在 QFormLayout::addRow 全部完成之后调用
void applyFormLayoutStyle(QFormLayout *form);
void applyAllFormLayouts(QWidget *root);

void applyComboBoxStyle(QComboBox *box);
void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx);

void applyLineEditStyle(QLineEdit *edit);
void applySpinBoxStyle(QSpinBox *spin);
void applyFormFieldStyles(QWidget *root);

} // namespace EbWidget

#endif // EB_WIDGET_UTIL_H
