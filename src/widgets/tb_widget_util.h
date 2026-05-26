#ifndef TB_WIDGET_UTIL_H
#define TB_WIDGET_UTIL_H

#include <QString>

class QComboBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QSpinBox;
class QWidget;

namespace TbWidget {

// 样式表下控件 sizeHint 的最小基准（ComboBox/按钮实际约 40–42px）
constexpr int kFormFieldHeight = 32;
constexpr int kFormRowVerticalSpacing = 12;
constexpr int kFormHorizontalSpacing = 12;

QLabel *createFormLabel(QWidget *parent, const QString &text, int rowHeight = 0);

void initFormRowWidget(QWidget *row);

int styledControlHeight(QWidget *widget);

class FormGridBuilder {
public:
    explicit FormGridBuilder(QWidget *parent);
    ~FormGridBuilder();

    QGridLayout *layout() const { return m_grid; }
    void addLabeledRow(const QString &labelText, QWidget *field);
    void addFieldRow(QWidget *field);

private:
    void prepareField(QWidget *field);
    int fieldRowHeight(QWidget *field) const;

    QWidget *m_parent = nullptr;
    QGridLayout *m_grid = nullptr;
    int m_row = 0;
};

void applyComboBoxStyle(QComboBox *box);
void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx);

void applyLineEditStyle(QLineEdit *edit);
void applySpinBoxStyle(QSpinBox *spin);
void applyFormFieldStyles(QWidget *root);

} // namespace TbWidget

#endif // TB_WIDGET_UTIL_H
