#ifndef EB_WIDGET_UTIL_H
#define EB_WIDGET_UTIL_H

class QComboBox;
class QFormLayout;
class QWidget;

namespace EbWidget {

// 须在 QFormLayout::addRow 全部完成之后调用
void applyFormLayoutStyle(QFormLayout *form);
void applyComboBoxStyle(QComboBox *box);
void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx);

} // namespace EbWidget

#endif // EB_WIDGET_UTIL_H
