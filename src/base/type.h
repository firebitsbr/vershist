#ifndef CPP_SEMVER_TYPE_HPP
#define CPP_SEMVER_TYPE_HPP

#include <string>
#include <vector>

namespace semver
{
  // ------------ exception types ------------------------------------ //

  struct semver_error : public std::runtime_error
  {
    semver_error(const std::string& msg) : std::runtime_error(msg) {}
  };

  // ------------ syntax types ------------------------------------ //

  struct syntax
  {
    /*
    * original NPM semver syntax grammar: https://docs.npmjs.com/misc/Syntax#range-grammar
    *
    * range-set  ::= range ( logical-or range ) *
    * logical-or ::= ( ' ' ) * '||' ( ' ' ) *
    * range      ::= hyphen | simple ( ' ' simple ) * | ''
    * hyphen     ::= partial ' - ' partial
    * simple     ::= primitive | partial | tilde | caret
    * primitive  ::= ( '<' | '>' | '>=' | '<=' | '=' | ) partial
    * partial    ::= xr ( '.' xr ( '.' xr qualifier ? )? )?
    * xr         ::= 'x' | 'X' | '*' | nr
    * nr         ::= '0' | ['1'-'9'] ( ['0'-'9'] ) *
    * tilde      ::= '~' partial
    * caret      ::= '^' partial
    * qualifier  ::= ( '-' pre )? ( '+' build )?
    * pre        ::= parts
    * build      ::= parts
    * parts      ::= part ( '.' part ) *
    * part       ::= nr | [-0-9A-Za-z]+
    */

    enum class comparator
    {
      eq, lt, lte, gt, gte, tilde, caret
    };

    /// default represents '*'
    struct xnumber
    {
      bool is_wildcard = true;
      int value = 0;
    };

    /// represents any type of 'simple', 'primitive', 'partial', 'tilde' or 'caret' from the grammar.
    /// The default value represents *.*.*
    struct simple
    {
      xnumber major;
      xnumber minor;
      xnumber patch;

      std::string pre = "";
      std::string build = "";

      comparator cmp = comparator::eq;
    };

    /// intersection set (i.e. AND conjunction )
    typedef std::vector< simple > range;

    /// union set (i.e. OR conjunction )
    typedef std::vector< range > range_set;
  };

  // ------------ semantic types ------------------------------------ //

  struct semantic
  {
    struct boundary
    {
      int major = 0;
      int minor = 0;
      int patch = 0;
      std::string pre = "";
      bool is_max = false;

      /// Represent a minimal version boundary
      static boundary min()
      {
        return {};
      }

      /// Represent a maximal version boundary
      static boundary max()
      {
        boundary b;
        b.is_max = true;
        return b;
      }
    };

    /// default interval is *.*.* := [min, max]
    struct interval
    {
      bool from_inclusive = true;
      bool to_inclusive   = true;
      boundary from = boundary::min();
      boundary to   = boundary::max();
    };

    /// interval set (i.e. OR conjunction )
    typedef std::vector< interval > interval_set;
  };

  bool operator ==(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    return (lhs.major == rhs.major) &&
      (lhs.minor == rhs.minor) &&
      (lhs.patch == rhs.patch) &&
      (lhs.pre == rhs.pre) &&
      (lhs.is_max == rhs.is_max);
  }

  bool operator !=(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    return !(lhs == rhs);
  }

  bool operator <(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    // TODO: improve the performance of comparison

    // XXX: is_max case
    if (lhs.is_max || rhs.is_max)
      return !lhs.is_max && rhs.is_max; // 1.2.3 < max

    if (lhs.major < rhs.major)      // 1.* < 2.*
      return true;

    if ((lhs.major == rhs.major) && // 1.2.* < 1.3.*
      (lhs.minor < rhs.minor))
      return true;

    if ((lhs.major == rhs.major) && // 1.2.3 < 1.2.4
      (lhs.minor == rhs.minor) &&
      (lhs.patch < rhs.patch))
      return true;

    // XXX: pre-release case
    if ((lhs.major == rhs.major) && // 1.2.3-alpha < 1.2.3
      (lhs.minor == rhs.minor) &&
      (lhs.patch == rhs.patch) &&
      (!lhs.pre.empty() && rhs.pre.empty()))
      return true;

    // XXX: pre-release case
    if ((lhs.major == rhs.major) && // 1.2.3-alpha < 1.2.3-beta
      (lhs.minor == rhs.minor) &&
      (lhs.patch == rhs.patch) &&
      (!lhs.pre.empty() && !rhs.pre.empty()) &&
      (lhs.pre < rhs.pre))
      return true;

    return false;
  }

  bool operator >(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    return (lhs != rhs) && !(lhs < rhs);
  }

  bool operator <=(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    return (lhs < rhs) || (lhs == rhs);
  }

  bool operator >=(const semantic::boundary& lhs, const semantic::boundary& rhs)
  {
    return (lhs > rhs) || (lhs == rhs);
  }

  bool operator ==(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    return (lhs.from_inclusive == rhs.from_inclusive) &&
      (lhs.to_inclusive == rhs.to_inclusive) &&
      (lhs.from == rhs.from) &&
      (lhs.to == rhs.to);
  }

  bool operator !=(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    return !(lhs == rhs);
  }

  bool operator <(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    if (lhs.to < rhs.from ||
      (lhs.to == rhs.from && (!lhs.to_inclusive || !rhs.from_inclusive)))
      return true;
    return false;
  }

  bool operator <=(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    return (lhs < rhs) || (lhs == rhs);
  }

  bool operator >(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    return (lhs != rhs) && !(lhs < rhs);
  }

  bool operator >=(const semantic::interval& lhs, const semantic::interval& rhs)
  {
    return (lhs > rhs) || (lhs == rhs);
  }
}

#endif