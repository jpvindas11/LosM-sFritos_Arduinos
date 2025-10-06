/// @copyright Los Más Fritos - 2025

#define TLB_SIZE 16

struct TLB_entry {
    int page_number;
    int frame_number;
};

class TLB {
public:
  TLB();
  ~TLB();
};