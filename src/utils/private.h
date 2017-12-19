#ifndef UTILS_PRIVATE_HEADER
#define UTILS_PRIVATE_HEADER

// creates private variables with different read/write access
// PRIV_VAR(TYPE, NAME)		// no access
// PRIV_VAR_R(TYPE, NAME)	// r-access
// PRIV_VAR_RW(TYPE, NAME)	// rw-access
// PRIV_VAR_P(TYPE, NAME)	// pointer, no access
// PRIV_VAR_P_R(TYPE, NAME)	// pointer, r-access
// PRIV_VAR_P_PR(TYPE, NAME)	// pointer, r-access to the pointer
// PRIV_VAR_P_RW(TYPE, NAME)	// pointer, rw-access
// PRIV_VAR_CP(TYPE, NAME)	// const pointer, no access
// PRIV_VAR_CP_R(TYPE, NAME)	// const pointer, r-access
// PRIV_VAR_CP_RW(TYPE, NAME)	// const pointer, rw-access
// PRIV_VAR_CP_PR(TYPE, NAME)	// const pointer, r-access to the pointer

// for all the following macros:
//   create a private variable named 'NAME_' of type 'TYPE' / 'TYPE*'
//   behind the context is 'private'
// for some macros (see examples)
//   function 'TYPE NAME()'
//   function 'set_NAME(NAME)'




// Example: 
//   PRIV_VAR(int, x);
// leads to (concerning the content):
//   private: int x_;
#define PRIV_VAR(TYPE, NAME) \
  private: \
    TYPE NAME ## _

// note: behind the context is 'private'
// Example: 
//   PRIV_VAR_RW(int, x);
// leads to (concerning the content):
//   private: int x_;
//   public: int x() const 	{ return this->x_; }
//   public: void x(int value)	{ this->x_ = value; }
#define PRIV_VAR_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return this->NAME ## _ ; } \
    inline void set_ ## NAME (TYPE const& value) \
    { this->NAME ## _ = value; return ; } \
  private: \
    TYPE NAME ## _

// Example: 
//   PRIV_VAR_R(int, x);
// leads to (concerning the content):
//   private: int x_;
//   public: int x() const 	{ return this->x_; }
#define PRIV_VAR_R(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return this->NAME ## _ ; } \
  private: \
    TYPE NAME ## _

// Example: 
//   PRIV_VAR_P(int, x);
// leads to (concerning the content):
//   private: int* x_;
#define PRIV_VAR_P(TYPE, NAME) \
  private: \
    TYPE * NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_P_R(int, x);
// leads to (concerning the content):
//   private: int* x_;
//   public: int const& x() const 	{ return *this->x_; }
#define PRIV_VAR_P_R(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
  private: \
    TYPE * NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_P_PR(int, x);
// leads to (concerning the content):
//   private: int* x_;
//   public: int const* x() const 	{ return this->x_; }
#define PRIV_VAR_P_PR(TYPE, NAME) \
  public: \
    inline TYPE const* NAME() const \
    { return this->NAME ## _ ; } \
  private: \
    TYPE * NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_P_RW(int, x);
// leads to (concerning the content):
//   private: int* x_;
//   public: int const& x() const 	{ return *this->x_; }
//   public: int& x() { return *this->x_; }
#define PRIV_VAR_P_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
    inline TYPE & NAME() \
    { return *this->NAME ## _ ; } \
  private: \
    TYPE * NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_MP_RW(int, x);
// leads to (concerning the content):
//   private: mutable int* x_;
//   public: int const& x() const 	{ return *this->x_; }
//   public: int& x() { return *this->x_; }
#define PRIV_VAR_MP_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
    inline TYPE & NAME() \
    { return *this->NAME ## _ ; } \
  private: \
    mutable TYPE * NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_CP(int, x);
// leads to (concerning the content):
//   private: int const* x_;
#define PRIV_VAR_CP(TYPE, NAME) \
  private: \
    TYPE const* NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_CP_R(int, x);
// leads to (concerning the content):
//   private: int const* x_;
//   public: int const& x() const 	{ return *this->x_; }
#define PRIV_VAR_CP_R(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
  private: \
    TYPE const* NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_CP_RW(int, x);
// leads to (concerning the content):
//   public: int const& x() const 	{ return *this->x_; }
//   public: void set_x(int const& x)	{ this->x_ = x; return ; }
//   private: int const* x_;
#define PRIV_VAR_CP_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
    inline void NAME(TYPE const& NAME) \
    { this->NAME ## _ = & NAME; return ; } \
  private: \
    TYPE const* NAME ## _ = nullptr

// Example: 
//   PRIV_VAR_CP_PR(int, x);
// leads to (concerning the content):
//   private: int const* x_;
//   public: int const* x() const 	{ return this->x_; }
#define PRIV_VAR_CP_PR(TYPE, NAME) \
  public: \
    inline TYPE const* NAME() const \
    { return this->NAME ## _ ; } \
  private: \
    TYPE const* NAME ## _ = nullptr



// protected



// Example: 
//   PROT_VAR_R(int, x);
// leads to (concerning the content):
//   private: int x_;
//   public: int x() const 	{ return this->x_; }
#define PROT_VAR_R(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return this->NAME ## _ ; } \
  protected: \
    TYPE NAME ## _

// Example: 
//   PROT_VAR_RW(int, x);
// leads to (concerning the content):
//   protected: int x_;
//   public: int x() const 	{ return this->x_; }
//   public: void x(int value)	{ this->x_ = value; }
#define PROT_VAR_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return this->NAME ## _ ; } \
    inline void set_ ## NAME (TYPE const& value) \
    { this->NAME ## _ = value; return ; } \
  protected: \
    TYPE NAME ## _

// Example: 
//   PROT_VAR_P_R(int, x);
// leads to (concerning the content):
//   protected: int* x_;
//   public: int const& x() const 	{ return *this->x_; }
//   public: int& x() 			{ return *this->x_; }
#define PROT_VAR_P_RW(TYPE, NAME) \
  public: \
    inline TYPE const& NAME() const \
    { return *this->NAME ## _ ; } \
    inline TYPE & NAME() \
    { return *this->NAME ## _ ; } \
  protected: \
    TYPE * NAME ## _ = nullptr


#endif // #ifndef UTILS_PRIVATE_HEADER
