%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% admissible/semi-stable semantics               %
% using non-/semi-normalized tree decompositions %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

1 { extend(R)  : childRow(R,N) } 1 :- childNode(N).

% Guess whether an element is in, out or attacking (attc)
0 { item(in(A)); attc(A) } 1       :- introduced(A).

% Join only arguments with the same color 
nIn(S,A)  :- childRow(S,N), bag(N,A), not childItem(S,in(A)).
nDef(S,A) :- childRow(S,N), bag(N,A), not childAuxItem(S,def(A)), not childAuxItem(S,attc(A)).
:- extend(S1), extend(S2), childItem(S1,in(A)),      nIn(S2,A).
:- extend(S1), extend(S2), childAuxItem(S1,def(A)),  nDef(S2,A).
:- extend(S1), extend(S2), childAuxItem(S1,attc(A)), nDef(S2,A).

% Inherit arguments that are in, defeated or attackers
item(in(A)) :- extend(S), childItem(S,in(A)),      current(A).
chdef(A)    :- extend(S), childAuxItem(S,def(A)),  current(A).
attc(A)     :- extend(S), childAuxItem(S,attc(A)), current(A).

% Set defeated arguments
auxItem(def(A))  :- current(A;B), att(B,A), item(in(B)).
auxItem(def(A))  :- chdef(A).

% Still remaining (undefeated) attackers
auxItem(attc(A)) :- attc(A), not auxItem(def(A)).

% Out-arguments are not allowed to be defeated/attackers 
out(A) :- not attc(A), not chdef(A), current(A).
:- auxItem(def(A)), out(A).
:- out(A), current(A), item(in(B)), att(A,B).

% Assure that the set is conflict-free  
:- item(in(A)), item(in(B)), att(A,B).

% Remove candidates that leave attackers undefeated
:- extend(S), childAuxItem(S,attc(A)), removed(A).

%opt stuff
optItem(A) :- item(in(A)).
optItem(A) :- auxItem(attc(A)).
optItem(A) :- auxItem(def(A)).

#show item/1.
#show auxItem/1.
#show extend/1.
#show optItem/1.
