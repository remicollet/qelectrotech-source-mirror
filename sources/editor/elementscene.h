/*
	Copyright 2006-2019 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ELEMENT_SCENE_H
#define ELEMENT_SCENE_H
#include <QtWidgets>
#include <QtXml>
#include "qgimanager.h"
#include "elementcontent.h"
#include "diagramcontext.h"
#include "nameslist.h"

class CustomElementPart;
class ElementEditionCommand;
class ElementPrimitiveDecorator;
class QETElementEditor;
class ESEventInterface;
class QKeyEvent;
/**
	This class is the canvas allowing the visual edition of an electrial element.
	It displays the various primitives composing the drawing of the element, the
	border due to its fixed size and its hotspot.
*/
class ElementScene : public QGraphicsScene
{
	friend class ChangePropertiesCommand;
	Q_OBJECT
	
		// enum
	public:
		enum Behavior { Normal, PasteArea, AddPart };
		enum ItemOption {
			SortByZValue = 1,
			IncludeTerminals = 2,
			IncludeHelperItems = 4,
			Selected = 8,
			NonSelected = 16,
			SelectedOrNot = 24
		};
		Q_DECLARE_FLAGS(ItemOptions, ItemOption)
	
		// constructors, destructor
	public:
		ElementScene(QETElementEditor *, QObject * = nullptr);
		~ElementScene() override;
	
	private:
		ElementScene(const ElementScene &);
	
		// attributes
	private:
		NamesList m_names_list; /// List of localized names
		QString m_informations; /// Extra informations
		QString m_elmt_type; /// element type
		DiagramContext m_elmt_kindInfo,
					   m_elmt_information; /// element kind info
		QGIManager m_qgi_manager;
		QUndoStack m_undo_stack;

		ESEventInterface *m_event_interface = nullptr;
		Behavior m_behavior;
		QETElementEditor *m_element_editor = nullptr;
	
		QGraphicsRectItem *m_paste_area;
		QRectF m_defined_paste_area;
		
		QString m_last_copied;
	
			/// Decorator item displayed when at least one item is selected
		ElementPrimitiveDecorator *m_decorator = nullptr;

		int m_x_grid,
			m_y_grid;
	
		// methods
	public:
		void setEventInterface (ESEventInterface *event_interface);
		void clearEventInterface();
		void setBehavior (ElementScene::Behavior);
		ElementScene::Behavior behavior() const;
		QPointF snapToGrid(QPointF point);
		void setNames(const NamesList &);
		NamesList names() const;
		QString informations() const;
		void setInformations(const QString &);
		QString elementType () const {return m_elmt_type;}
		DiagramContext elementKindInfo () const {return m_elmt_kindInfo;}
		DiagramContext elementInformation() const {return m_elmt_information;}
		virtual int xGrid() const;
		virtual int yGrid() const;
		virtual void setGrid(int, int);
		virtual const QDomDocument toXml(bool = true);
		virtual QRectF boundingRectFromXml(const QDomDocument &);
		virtual void fromXml(const QDomDocument &, const QPointF & = QPointF(), bool = true, ElementContent * = nullptr);
		virtual void reset();
		virtual QList<CustomElementPart *> primitives() const;
		virtual QList<QGraphicsItem *> zItems(ItemOptions options = ItemOptions(SortByZValue | IncludeTerminals | SelectedOrNot)) const;
		virtual ElementContent selectedContent() const;
		virtual void getPasteArea(const QRectF &);
		QRectF elementSceneGeometricRect () const;
		bool containsTerminals() const;
		QUndoStack &undoStack();
		QGIManager &qgiManager();
		static bool clipboardMayContainElement();
		bool wasCopiedFromThisElement(const QString &);
		void cut();
		void copy();
		QETElementEditor* editor() const;
		void setElementInfo(const DiagramContext& dc);
	
	protected:
		void mouseMoveEvent         (QGraphicsSceneMouseEvent *) override;
		void mousePressEvent       (QGraphicsSceneMouseEvent *) override;
		void mouseReleaseEvent     (QGraphicsSceneMouseEvent *) override;
		void mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event) override;
		void keyPressEvent         (QKeyEvent *event) override;
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

		void drawForeground(QPainter *, const QRectF &) override;
	
	private:
		QRectF elementContentBoundingRect(const ElementContent &) const;
		bool applyInformations(const QDomDocument &);
		ElementContent loadContent(const QDomDocument &);
		ElementContent addContent(const ElementContent &);
		ElementContent addContentAtPos(const ElementContent &, const QPointF &);
		void addPrimitive(QGraphicsItem *);
		void initPasteArea();
		static bool zValueLessThan(QGraphicsItem *, QGraphicsItem *);
		QMutex *m_decorator_lock;
		void centerElementToOrigine();
	
	public slots:
		void slot_select(const ElementContent &);
		void slot_selectAll();
		void slot_deselectAll();
		void slot_invertSelection();
		void slot_delete();
		void slot_editNames();
		void slot_editAuthorInformations();
		void slot_editProperties();
		void managePrimitivesGroups();
		void stackAction(ElementEditionCommand *);
	
	signals:
			/// Signal emitted after one or several parts were added
		void partsAdded();
			/// Signal emitted after one or several parts were removed
		void partsRemoved();
			/// Signal emitted when the zValue of one or several parts change
		void partsZValueChanged();
			/// Signal emitted when users have defined the copy/paste area
		void pasteAreaDefined(const QRectF &);
			/// Signal emitted when need zoomFit
		void needZoomFit();
		void elementInfoChanged();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ElementScene::ItemOptions)


/**
	@param nameslist New set of naes for the currently edited element
*/
inline void ElementScene::setNames(const NamesList &nameslist) {
	m_names_list = nameslist;
}

/**
	@return the list of names of the currently edited element
*/
inline NamesList ElementScene::names() const {
	return(m_names_list);
}

/**
	@return extra informations of the currently edited element
*/
inline QString ElementScene::informations() const {
	return(m_informations);
}

/**
	@param infos new extra information for the currently edited element
*/
inline void ElementScene::setInformations(const QString &infos) {
	m_informations = infos;
}

#endif
