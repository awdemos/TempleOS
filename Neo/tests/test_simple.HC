class CPoint {
  I64 x,y;
  U0 Draw() {
    Print("Point at %d,%d\n",x,y);
  }
};

CPoint origin;

U0 Main(I64 argc, U8 **argv) {
  CPoint *p = &origin;
  p->x = 10;
  p->Draw();
}
