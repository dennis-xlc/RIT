#!/usr/bin/python

import fileinput

opts = {}
output = []
s_inst = """
    {
        'opt': {%s},
        'fmt': '%s'
    }"""

print "#!/usr/bin/python"

for l in fileinput.input():
    #print l.strip()
    if l.startswith('#'):
        if l.startswith('#dt:'):
            subs = l[len('#dt:'):].strip().split('/')
            #print subs
            opts['dt'] = tuple(['\'%s\'' % s for s in subs])
        elif l.startswith('#size:'):
            subs = l[len('#size:'):].strip().split('/')
            #print subs
            opts['size'] = tuple(subs)
        else:
            pass
            #print "#[warning] skip line %s" % l
    elif l.startswith('V'):
        inst = l.strip()
        s_opts = []
        if '<dt>' in inst:
            s_dt = ', '.join(opts['dt'])
            s_opts.append('\'dt\': (%s)' % s_dt)
        if '<size>' in inst:
            s_size = ', '.join(opts['size'])
            s_opts.append('\'size\': (%s)' % s_size)
        output.append(s_inst % (','.join(s_opts), inst))
print "NEON_INST = [%s]" % ",".join(output)

