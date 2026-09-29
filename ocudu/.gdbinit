# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

############################################
#            Pretty-Printers
############################################

# If you add a pretty printer, please create the corresponding test in
# tests/utils/gdb/pretty_printers

python

sys.path.append('./utils/gdb/python/')

from ocudu.printers import register_printers
register_printers(gdb.current_objfile())
