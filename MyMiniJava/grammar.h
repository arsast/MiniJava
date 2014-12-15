#pragma once
class IVisitor;

//http://www.ibm.com/developerworks/library/l-flexbison/
struct CCodeInfo {
	int firstLine;
	int firstColumn;
	int lastLine;
	int lastColumn;

	CCodeInfo() {}
	CCodeInfo( const int begLine, const int begPos, const int endLine, const int endPos ) : firstLine( begLine ),
		firstColumn( begPos ),
		lastLine( endLine ),
		lastColumn( endPos ) { }
};

// �������� ��������� ���������
class IProgram {
public:
	virtual ~IProgram() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������� �����
class IMainClass {
public:
	virtual ~IMainClass() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ �������
class IClassDeclList {
public:
	virtual ~IClassDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// �������� ������
class IClassDecl {
public:
	virtual ~IClassDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ���������� ����������
class IVarDeclList {
public:
	virtual ~IVarDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���������� ����������
class IVarDecl {
public:
	virtual ~IVarDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ������� ������
class IMethodDeclList {
public:
	virtual ~IMethodDeclList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ����� ������
class IMethodDecl {
public:
	virtual ~IMethodDecl() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������ ����������
class IFormalList {
public:
	virtual ~IFormalList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���
class IType {
public:
	virtual ~IType() {}
	virtual std::string GetName() const = 0;
	virtual void Accept( IVisitor* ) const = 0;
};

// ������������������ ������
class IStmtList {
public:
	virtual ~IStmtList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ���� ������� (��� ����)
class IStmt {
public:
	virtual ~IStmt() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// ������������������ ��������� (��������� �������)
class IExprList {
public:
	virtual ~IExprList() {}
	virtual void Accept( IVisitor* ) const = 0;
};

// �������������� ���������
class IExpr {
public:
	virtual ~IExpr() {}
	virtual void Accept( IVisitor* ) const = 0;
};