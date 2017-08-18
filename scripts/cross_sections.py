from inspect import getsourcefile
import re, os

def get_dsid(in_file):
    ds_regex = re.compile('([0-9]{2,})')
    match = ds_regex.search(in_file).group(1)
    return int(match)

class CrossSections:
    def __init__(self, file_name=None, lumi_fb=20):
        self.datasets = {}
        self.lumi_fb = lumi_fb
        if file_name is None:
            this_dir = os.path.dirname(getsourcefile(CrossSections))
            file_name = '{}/../data/xsec.txt'.format(this_dir)
        with open(file_name) as infile:
            for rawline in infile:
                self._add_line(rawline)

    def _add_line(self, rawline):
        line = rawline.strip()
        if not line or line.startswith('#'):
            return
        shortname, dsid_s, xsec_nb_s, filteff_s, evts_s = line.split()
        self.datasets[int(dsid_s)] = {
            'xsec_fb': float(xsec_nb_s) * 1e6,
            'filteff': float(filteff_s),
            'nevt': float('nan') if evts_s == '?' else float(evts_s),
            'shortname': shortname}

    def get_weight(self, dsid):
        rec = self.datasets[dsid]
        return self.lumi_fb * rec['xsec_fb'] * rec['filteff'] / rec['nevt']

    def is_signal(self, dsid):
        rec = self.datasets[dsid]
        if rec['shortname'].startswith('JZ'):
            return False
        return True
