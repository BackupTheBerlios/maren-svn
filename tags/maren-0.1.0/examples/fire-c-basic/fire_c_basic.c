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
is_the_house_burning( void* dummy, const void* fact )
{
  return ((const Fact*)fact)->f.house.is_burning;
}

static bool
is_phone_not_in_house( void* dummy,
		       const Fact* phone,
		       const Fact* house )
{
  return phone->f.phone.location != &(house->f.house);
}

static void
call_the_firefighters( void* dummy,
		       MarenContext* ctx,
		       const MarenActiveSet* phone_n_house )
{

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
  maren_node_set_dbg_info( node, "Is it a house?" );

  MarenStdSingleTestData is_phone = { offsetof( Fact, type ), { FT_PHONE } };
  node = maren_rule_add_single_check( &rule, 1,
				      maren_std_single_test_int_eq,
				      &is_phone, NULL );
  maren_node_set_dbg_info( node, "Is it a phone?" );

  node = maren_rule_add_single_check( &rule, 0,
				      is_the_house_burning,
				      NULL, NULL );
  maren_node_set_dbg_info( node, "Is the house burninge?" );

  node = maren_rule_add_double_check(&rule,
				     1, 0,
				     (MarenDoubleCheckFn)is_phone_not_in_house,
				     NULL, NULL );
  maren_node_set_dbg_info( node, "Is the phone in the house?" );

  node = maren_rule_set_body( &rule, 0,
			      call_the_firefighters,
			      "112", NULL );
  maren_node_set_dbg_info( node, "Call the firefighters!" );

  maren_context_add_rule( &ctx, &rule );
  maren_ctx2dot( stdout, &ctx, "fire_c_basic" );

  return rc;
}
