#include "tableview.h"

CTableView::CTableView()
{
}

void CTableView::keyPressEvent(QKeyEvent *e)
{
	switch ( e->key() )
	{
	case Qt::Key_Escape:
	{
		setCurrentIndex( QModelIndex() );
		break;
	}

	case Qt::Key_Right:
	{
		QAbstractItemModel* model = QAbstractItemView::model();

		if ( model )
		{
			QModelIndex currentIndex = QAbstractItemView::currentIndex();

			if ( currentIndex.isValid() ) // If there is a selected entry...
			{
				int currentRow = currentIndex.row();
				// ...and if we're not at the end of the list...
				if ( ++currentRow != model->rowCount( QModelIndex() ) )
				{
					// Makes sure the condition above is sufficient.
					Q_ASSERT( currentRow < model->rowCount( QModelIndex() ) && currentRow > 0 );

					// ...select the item that comes after the current.
					setCurrentIndex( model->index( currentRow, 0 ) );
				}
			}
			else if ( model->rowCount( QModelIndex() ) ) // If no selection exists and there are rules...
			{
				// ...select the first entry.
				setCurrentIndex( model->index( 0, 0 ) );
			}
		}

		break;
	}

	case Qt::Key_Left:
	{
		QAbstractItemModel* model = QAbstractItemView::model();

		if ( model )
		{
			QModelIndex currentIndex = QAbstractItemView::currentIndex();

			if ( currentIndex.isValid() ) // If there is a selected entry...
			{
				int currentRow = currentIndex.row();
				// ...and if we're not at the head of the list...
				if ( --currentRow != -1 )
				{
					// Makes sure the condition above is sufficient.
					Q_ASSERT( currentRow < model->rowCount( QModelIndex() ) && currentRow > -1 );

					// ...select the previous item.
					setCurrentIndex( model->index( currentRow, 0 ) );
				}
			}
			else if ( model->rowCount( QModelIndex() ) ) // If no selection exists and there are rules...
			{
				// ...select the last entry.
				setCurrentIndex( model->index( model->rowCount( QModelIndex() ) - 1, 0 ) );
			}
		}

		break;
	}

	case Qt::Key_Home:
	{
		QAbstractItemModel* model = QAbstractItemView::model();

		if ( model && model->rowCount( QModelIndex() ) )
			setCurrentIndex( model->index( 0, 0 ) );

		break;
	}

	case Qt::Key_End:
	{
		QAbstractItemModel* model = QAbstractItemView::model();

		if ( model )
		{
			int lastRow = model->rowCount( QModelIndex() );
			if ( lastRow != 0 ) // If there are Items in the list...
				setCurrentIndex( model->index( --lastRow, 0 ) );
		}
		break;
	}

	default:
		QTableView::keyPressEvent( e );
	}
}
