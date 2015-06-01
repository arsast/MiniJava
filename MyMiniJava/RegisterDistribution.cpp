﻿// Автор: Федюнин Валерий
// Набор классов для распределения регистров по переменным

#include "RegisterDistribution.h"

#include <assert.h>
#include <iostream>

namespace Assembler {

CInterferenceGraph::CInterferenceGraph( const std::list<const IAsmInstr*>& asmFunction,
	const std::vector<const std::string>& registers ) : asmFunction( asmFunction ), liveInOut( asmFunction ), 
	registers( registers )
{
	do {
		std::cerr << asmFunction.size() << std::endl;
		int cmdIndex = 0;
		if( !uncoloredNodes.empty() ) {
			std::cout << "REGENERATING!!!" << std::endl;
			regenerateCode();
			uncoloredNodes.clear();
			edges.clear();
			nodes.clear();
			nodeMap.clear();
			while( !pulledNodes.empty() ) {
				pulledNodes.pop();
			}
			uncoloredNodes.clear();
			liveInOut = CLiveInOutCalculator( asmFunction );
		}
		for( auto cmd : asmFunction ) {
			if( cmdIndex == 5 ) {
				cmdIndex = 5;
			}
			if( dynamic_cast< const CMove* >( cmd ) == nullptr ) {
				// для каждой не move инструкции добавить ребра между всеми такими переменными a и b
				// где a принадлежит определяемым в данной инструкции переменным
				// b - из множества liveOut
				for( auto a : liveInOut.GetDefines( cmdIndex ) ) {
					for( auto b : liveInOut.GetLiveOut( cmdIndex ) ) {
						addNode( a );
						addNode( b );
						addEdge( a, b );
					}
				}
			} else {
				// для каждой move инструкции добавить ребра между всеми такими переменными a и b
				// где a - куда делается MOVE (c->a)
				// b из множества liveOut
				std::string a = dynamic_cast< const CMove* >( cmd )->Destination()->GetCurrent()->ToString();
				for( auto b : liveInOut.GetLiveOut( cmdIndex ) ) {
					addNode( a );
					addNode( b );
					addEdge( a, b );
				}
				if( dynamic_cast< const CMove* >( cmd )->Source() != nullptr ) {
					std::string b = dynamic_cast< const CMove* >( cmd )->Source()->GetCurrent()->ToString();
					addNode( a );
					addNode( b );
					addMoveEdge( a, b );
				}
			}
			for( auto a : liveInOut.GetDefines( cmdIndex ) ) {
				addNode( a );
			}
			for( auto a : liveInOut.GetUses( cmdIndex ) ) {
				addNode( a );
			}
			++cmdIndex;
		}
	} while( !paint() );
}


// добавляет вершину в граф, если таковой еще нет
void CInterferenceGraph::addNode( const std::string& name )
{
	if( nodeMap.find( name ) != nodeMap.end() ) {
		return;
	}
	nodeMap.insert( std::make_pair( name, nodes.size() ) );
	nodes.emplace_back();
	for( int i = 0; i < edges.size(); ++i ) {
		edges[i].push_back( ET_NoEdge );
	}
	edges.emplace_back( nodes.size(), ET_NoEdge );
}


// добавляет move-ребро в граф
void CInterferenceGraph::addMoveEdge( const std::string& from, const std::string& to )
{
	int u = nodeMap[from];
	int v = nodeMap[to];
	if( u == v ) {
		return;
	}
	// MOVE-ребро добавляется только тогда, когда не было никакого ребра между этими вершинами
	if( edges[u][v] == ET_NoEdge ) {
		edges[u][v] = ET_MoveEdge;
		edges[v][u] = ET_MoveEdge;
	}
}


// добавляет простую зависимость в граф
void CInterferenceGraph::addEdge( const std::string& from, const std::string& to )
{
	int u = nodeMap[from];
	int v = nodeMap[to];
	if( u == v ) {
		return;
	}
	edges[u][v] = ET_Edge;
	edges[v][u] = ET_Edge;
}


// раскрасить граф
bool CInterferenceGraph::paint()
{
	addRegisterColors();
	while( hasNonColoredNonStackedNodes() ) {
		int node = getColorableNode();
		if( node == -1 ) {
			node = getMaxInterferingNode();
			uncoloredNodes.insert( node );
		}
		pulledNodes.push( node );
		nodes[node].InStack = true;
	}
	if( !uncoloredNodes.empty() ) {
		return false;
	}
	while( !pulledNodes.empty() ) {
		int currNode = pulledNodes.top();
		pulledNodes.pop();
		std::vector<char> usedColors( registers.size(), 0 );
		for( int i = 0; i < nodes.size(); ++i ) {
			if( edges[currNode][i] != ET_NoEdge  &&  nodes[i].Color != -1  &&  nodes[i].Color < usedColors.size() ) {
				usedColors[nodes[i].Color] = 1;
			}
		}
		for( int i = 0; i < usedColors.size(); ++i ) {
			if( !usedColors[i] ) {
				nodes[currNode].Color = i;
				break;
			}
		}
	}
	return true;
}


// красит вершины, соответствующие регистрам
void CInterferenceGraph::addRegisterColors()
{
	for( int i = 0; i < registers.size(); ++i ) {
		auto regNode = nodeMap.find( registers[i] );
		if( regNode != nodeMap.end() ) {
			// если в графе есть вершина, соответствующая этому регистру, 
			// то она должна быть покрашена до запуска основного алгоритма покраски
			nodes[regNode->second].Color = i;
		}
	}

	for( auto it : nodeMap ) {
		if( it.first.substr( it.first.length() - 3 ) == "_SP" ) {
			// красим насильно в ESP
			nodes[it.second].Color = 6;
		}
		if( it.first.substr( it.first.length() - 3 ) == "_FP" ) {
			// красим насильно в EBP
			nodes[it.second].Color = 7;
		}
		if( it.first.substr( it.first.length() - 3 ) == "_RV" ) {
			// красим насильно в EDI
			nodes[it.second].Color = 5;
		}
	}
}


// остались ли в графе вершины, которые не положены в стек
// и не имеют цвет
bool CInterferenceGraph::hasNonColoredNonStackedNodes() const
{
	for( int i = 0; i < nodes.size(); ++i ) {
		if( nodes[i].Color == -1 && !nodes[i].InStack ) {
			return true;
		}
	}
	return false;
}


// номер вершины, которую в данный момент возможно покрасить
// или -1 если такой вершины в графе нет
int CInterferenceGraph::getColorableNode() const
{
	int colorsNum = registers.size();

	for( int i = 0; i < edges.size(); ++i ) {
		if( nodes[i].Color == -1  &&  getNeighbourNum( i ) < colorsNum  &&  !nodes[i].InStack ) {
			return i;
		}
	}

	return -1;
}


// номер вершины с наибольшим кол-вом соседей
int CInterferenceGraph::getMaxInterferingNode() const
{
	int maxNeighbour = -1;
	int nodeIndex = -1;
	for( int i = 0; i < nodes.size(); ++i ) {
		int currNeighbour = getNeighbourNum( i );
		if( currNeighbour > maxNeighbour  &&  !nodes[i].InStack ) {
			maxNeighbour = currNeighbour;
			nodeIndex = i;
		}
	}
	return nodeIndex;
}


// кол-во соседей у текущей вершины
// не учитывает вершины, расположенные на стеке
int CInterferenceGraph::getNeighbourNum( int nodeIndex ) const
{
	int neighbours = 0;
	for( int i = 0; i < edges[nodeIndex].size(); ++i ) {
		if( edges[nodeIndex][i] != ET_NoEdge  &&  !nodes[i].InStack ) {
			neighbours++;
		}
	}
	return neighbours;
}


// перегенерировать код, чтобы появилась раскраска
void CInterferenceGraph::regenerateCode()
{
	std::list<const IAsmInstr*> newCode;
	for( auto it : asmFunction ) {
		if( it->Source() != nullptr  &&  it->Source()->GetCurrent() != nullptr  &&
			nodeMap.find( it->Source()->GetCurrent()->ToString() ) != nodeMap.end() ) 
		{
			int varIndex = nodeMap.find( it->Source()->GetCurrent()->ToString() )->second;
			if( uncoloredNodes.find( varIndex ) != uncoloredNodes.end() ) {
				bool isMove = false;
				if( dynamic_cast< const Assembler::CMove* >( it ) != nullptr ) {
					isMove = true;
				}
				Temp::CTemp* buff = new Temp::CTemp();
				newCode.push_back( new Assembler::CMove( "mov 'd0, 's0\n", new Temp::CTempList( buff, 0 ), it->Source() ) );
				if( isMove ) {
					newCode.push_back( new Assembler::CMove( "mov 'd0, 's0\n", it->Destination(), new Temp::CTempList( buff, 0 ) ) );
				} else {
					const Assembler::COper* cmd = dynamic_cast< const Assembler::COper* >( it );
					newCode.push_back( new Assembler::COper( cmd->GetOperator() + " 's0\n", it->Destination(), new Temp::CTempList( buff, 0 ) ) );
				}
			} else {
				newCode.push_back( it );
			}
		} else {
			newCode.push_back( it );
		}
	}
	asmFunction = newCode;
	newCode.clear();
	for( auto it : asmFunction ) {
		if( it->Destination() != nullptr  &&  it->Destination()->GetCurrent() != nullptr  &&
			nodeMap.find( it->Destination()->GetCurrent()->ToString() ) != nodeMap.end() ) {
			int varIndex = nodeMap.find( it->Destination()->GetCurrent()->ToString() )->second;
			if( uncoloredNodes.find( varIndex ) != uncoloredNodes.end() ) {
				const Assembler::CMove* cmd = dynamic_cast< const Assembler::CMove* >( it );
				assert( cmd != nullptr );
				Temp::CTemp* buff = new Temp::CTemp();
				newCode.push_back( new Assembler::CMove( "mov 'd0, 's0\n", new Temp::CTempList( buff, 0 ), it->Source() ) );
				newCode.push_back( new Assembler::CMove( "mov 'd0, 's0\n", it->Destination(), new Temp::CTempList( buff, 0 ) ) );
			} else {
				newCode.push_back( it );
			}
		} else {
			newCode.push_back( it );
		}
	}
}


const std::list<const IAsmInstr*>& CInterferenceGraph::GetCode() const
{
	return asmFunction;
}


std::map<std::string, std::string> CInterferenceGraph::GetColors()
{
	std::map<std::string, std::string> res;
	registers.push_back( "ESP" );
	registers.push_back( "EBP" );
	for( auto it : nodeMap ) {
		res.insert( std::make_pair( it.first, registers[nodes[it.second].Color] ) );
	}
	registers.pop_back();
	registers.pop_back();
	return res;
}

//======================================================================================================================



} // namespace Assembler