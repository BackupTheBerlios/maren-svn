#include <stdbool.h>
#include <maren/maren.h>

typedef struct {
  bool is_burning;
} House;

typedef struct {
  House* location;
} Phone;

typedef enum {
  FT_HOUSE,
  FT_PHONE,
} FactType;

typedef struct {
  FactType type;
  union {
    House house;
    Phone phone;
  } f;
} Fact;

static bool
is_the_house_burning( void* dumm, const void* fact )
{
  return ((Fact*)fact)->is_burning;
}

int main( int argc, char *argv[] )
{
  int rc = 0;
  MarenNodePtr node;
  MarenContext ctx = MAREN_CONTEXT_INIT;
  MarenDList rule = MAREN_DLIST_INIT;

  MarenStdSingleTestData is_house = { offsetof( Fact, type ), { FT_HOUSE } };
  node = maren_rule_add_single_check( &rule, 0,
				      maren_std_single_test_int_eq,
				      &is_house, NULL );

  MarenStdSingleTestData is_phone = { offsetof( Fact, type ), { FT_PHONE } };
  node = maren_rule_add_single_check( &rule, 1,
				      maren_std_single_test_int_eq,
				      &is_phone, NULL );

  node = maren_rule_add_single_check( &rule, 0,
				      is_the_house_burning,
				      NULL, NULL );
  return rc;
}
