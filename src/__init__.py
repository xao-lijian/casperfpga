"""
control and monitor fpga-based casper designs.
"""

# import all the main classes that we'll use often
try:
    from . import progska  # type: ignore
except ImportError:  # pragma: no cover
    # progska is a compiled extension used for fast SKARAB programming.
    # Allow the rest of the library to be used even if the extension is not built.
    progska = None

from .bitfield import Bitfield, Field
from .katadc import KatAdc
from .casperfpga import CasperFpga
from .transport_katcp import KatcpTransport
from .transport_tapcp import TapcpTransport
from .transport_skarab import SkarabTransport
from .transport_itpm import ItpmTransport
from .transport_redis import RedisTapcpTransport
from .transport_localpcie import LocalPcieTransport
from .transport_remotepcie import RemotePcieTransport
from .transport_alveo import AlveoTransport
from .memory import Memory
from .network import IpAddress, Mac
from .qdr import Qdr
from .register import Register
from .sbram import Sbram
from .snap import Snap
from .snapadc import SnapAdc
from .tengbe import TenGbe
from . import skarab_fileops

# BEGIN VERSION CHECK
# Prefer the installed package version (works for wheels, editable installs, etc.)
try:
    from importlib.metadata import PackageNotFoundError, version  # Python 3.8+
except ImportError:  # pragma: no cover
    from importlib_metadata import PackageNotFoundError, version  # type: ignore

try:
    __version__ = version("casperfpga")
except PackageNotFoundError:  # pragma: no cover
    # Imported directly from a source tree (not installed).
    import time as _time
    __version__ = "0.0+unknown.{}".format(_time.strftime("%Y%m%d%H%M"))
# END VERSION CHECK

name = "casperfpga"

# end
