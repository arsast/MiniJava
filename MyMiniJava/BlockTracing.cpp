#include "BlockTracing.h"
#include <set>

namespace Canon {

	using namespace IRTree;

	Tracer::Tracer() 
	{
		doneLabel = new Temp::CLabel();
	}

	void Tracer::findBlocks( const IRTree::CStmtList* list ) {
		for( const IRTree::CStmtList* curVertex = list; !curVertex;  ) {
			const CLabel* label = dynamic_cast<const CLabel*>( curVertex->GetCurrent() );
			blockSequence.push_back( CBlock() );
			int last = blockSequence.size() - 1;

			//����� �����, ���� ��� ���� ������� �� ��� ����, ���� ���, �� ������� ���������
			if( label ) {
				blockSequence[last].rootLabel = label->label->ToString();
				blockSequence[last].stms.push_back( curVertex->GetCurrent() );
			} else {
				std::string newLabelName( "RandomLabel" + std::to_string( std::rand() ) );
				blockSequence[last].stms.push_back( new CLabel( new Temp::CLabel( newLabelName ) ) );
				blockSequence[last].rootLabel = newLabelName;
			}
			//���� � ���������� stmt
			curVertex = static_cast<const CStmtList*>( list->GetNextStmts() );

			//������������ ��� ��������� stmts
			while( true ) {
				if( dynamic_cast<const CJump*>( curVertex->GetCurrent() ) ||  
					dynamic_cast<const CCJump*>( curVertex->GetCurrent() ) ) { 
					//���� jump ��� cjump, �� ������ ��������� �� � ����������, ��� ���� �������� ��������� � �� stmt � ������
					blockSequence[last].stms.push_back( curVertex->GetCurrent() );
					curVertex = static_cast<const CStmtList*>( curVertex->GetNextStmts() );
					break;					
				} else if( dynamic_cast<const CLabel*>( curVertex->GetCurrent() ) ) {
					//���� ��� �����-�� �����, �� ��������� � �������� ���� jump �� ��� � ����� �������
					//�� �� ������ curVertex, �.�. ����� ����� ����� ����������
					blockSequence[last].stms.push_back( new CJump( dynamic_cast<const CLabel*>( curVertex->GetCurrent() )->label ) ); 
					break;
				} else if( !curVertex->GetCurrent() ) {
					//���� ��� null, �� ������ ������ ��������, �� ��������� ���� �� ������������� jump ��� cjump � 
					//��������� jump �� ����� done
					blockSequence[last].stms.push_back( new CJump( doneLabel ) );
					curVertex = static_cast<const CStmtList*>( curVertex->GetNextStmts() );
					break;
				} else {
					//� ��������� ������� ��������� stmt � ��������� ����
					blockSequence[last].stms.push_back( curVertex->GetCurrent() );
					curVertex = static_cast<const CStmtList*>( curVertex->GetNextStmts() );
				}
			}
		}
	}

	void Tracer::sortBlocks() {
	}
}