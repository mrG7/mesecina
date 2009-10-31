#ifndef MESECINA_QSPINBOXEDITOR_H
#define MESECINA_QSPINBOXEDITOR_H

#include <QtGui/QItemDelegate>
#include <QtCore/QModelIndex>
//#include <QSize>
#include <QtGui/QSpinBox>

class Settings_table_model;


 class QSpinBoxEditor : public QItemDelegate
 {
     Q_OBJECT

 public:
     QSpinBoxEditor(Settings_table_model * table_model, QObject *parent = 0);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

	 Settings_table_model *table_model;
	 QModelIndex* current_index;
private slots:
	void valueChanged (int i); 

 };

#endif // MESECINA_QSPINBOXEDITOR_H