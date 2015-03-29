#include "IntermidRepresent.h"


namespace Translate {

class ISubtreeWrapper {
public:
	virtual ~ISubtreeWrapper() { };
	virtual const IRTree::IExpr* ToExp() const = 0; // ��� Expr
	virtual const IRTree::IStmt* ToStm() const = 0; // ��� Stm
	virtual const IRTree::IStmt* ToConditional( const Temp::CLabel* t, const Temp::CLabel* f ) const = 0; // ��� if/jump
};

}