program Hello;
const max = 100;
var size :integer;
type x = integer;
var a : array [1..max] of x;
procedure input();
var i : integer;
begin
    readLn(size);
    for i := 1 to size do
        readLn(a[i]);
end;

procedure findmax();
var i : integer;
var m : integer;
begin
    m := a[1];
    for i := 2 to size do 
        if a[i] > m then
            m := a[i];
        
    for i := 1 to size do
        if a[i] = m then
            writeln(i);
end;

begin
  writeln ('Hello World');
  input();
  findmax();
end.