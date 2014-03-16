""""
Meaningful Scales Detection: an Unsupervised Noise Detection Algorithm for \
Digital Contours
Demo Editor: B. Kerautret
"""

from lib import base_app, build, http, image
from lib.misc import app_expose, ctime
from lib.base_app import init_app
import cherrypy
from cherrypy import TimeoutError
import os.path
import shutil

class app(base_app):
    """ template demo app """
    title = "Meaningful Scales Detection: an Unsupervised Noise "+\
            "Detection Algorithm for Digital Contours"
    xlink_article = 'http://www.ipol.im/'
    xlink_src =  'http://www.ipol.im/pub/pre/75/meaningfulscaleDemo.tgz'
    demo_src_filename  = 'meaningfulscaleDemo.tar.gz'
    demo_src_dir  = 'meaningfulscaleDemo'
    
    input_nb = 1 # number of input images
    input_max_pixels = 500000 # max size (in pixels) of an input image
    input_max_weight = 1 * 1024 * 1024 # max size (in bytes) of an input file
    input_dtype = '3x8i' # input image expected data type
    input_ext = '.png'   # input image expected extension (ie file format)
    is_test = True       # switch to False for deployment


    def __init__(self):
        """
        app setup
        """
        # setup the parent class
        base_dir = os.path.dirname(os.path.abspath(__file__))
        base_app.__init__(self, base_dir)
        

        # select the base_app steps to expose
        # index() is generic
        app_expose(base_app.index)
        app_expose(base_app.input_select)
        app_expose(base_app.input_upload)
        # params() is modified from the template
        app_expose(base_app.params)
        # run() and result() must be defined here

    def build(self):
        """
        program build/update
        """
        # store common file path in variables
        tgz_file = self.dl_dir + self.demo_src_filename
        prog_names = ["meaningfulScaleEstim"]
        script_names = ["applyMS.sh", "convert.sh", "convertFig.sh", \
                        "transformBG.sh"]
        prog_bin_files = []
        
        for f in prog_names:
            prog_bin_files.append(self.bin_dir+ f)            

        log_file = self.base_dir + "build.log"
        # get the latest source archive
        build.download(self.xlink_src, tgz_file)
        
        # test if the dest file is missing, or too old
        if (os.path.isfile(prog_bin_files[0])
            and ctime(tgz_file) < ctime(prog_bin_files[0])):
            cherrypy.log("not rebuild needed",
                         context='BUILD', traceback=False)
        else:
            # extract the archive
            build.extract(tgz_file, self.src_dir)
            # build the program
            build.run("mkdir %s;" %(self.src_dir+ self.demo_src_dir+"/build"), \
                      stdout=log_file)
            build.run("cd %s; cmake ..  -DCMAKE_BUILD_TYPE=Release \
                     -DBUILD_TESTING=false  ; make -j 4" %(self.src_dir+ \
                                                          self.demo_src_dir+\
                                                          "/build"), 
                                                          stdout=log_file) 
            
            # save into bin dir
            if os.path.isdir(self.bin_dir):
                shutil.rmtree(self.bin_dir)
            os.mkdir(self.bin_dir)
            shutil.copy(self.src_dir + self.demo_src_dir + \
                        "/build/demoIPOL/meaningfulScaleEstim",  self.bin_dir)
            for f in script_names :
                shutil.copy(self.src_dir + os.path.join(self.demo_src_dir, \
                            "demoIPOL", f), self.bin_dir)
            
            # copy annex file : pgm2freeman (extraction of contours)
            shutil.copy(self.src_dir + self.demo_src_dir+ \
                        "/build/bin/pgm2freeman",  self.bin_dir)            

            # copy Dynamic lib
            shutil.copy(self.src_dir + self.demo_src_dir+ \
                        "/build/src/libImaGene.so", self.bin_dir)

            # cleanup the source dir
            shutil.rmtree(self.src_dir)
        
        return



    @cherrypy.expose
    @init_app
    def wait(self, **kwargs):
        """
        params handling and run redirection
        """
        # save and validate the parameters
        try:
            self.cfg['param'] = {'t' : float(kwargs['t']),
                                 'm' : float(kwargs['m'])}
        except ValueError:
            return self.error(errcode='badparams',
                              errmsg="The parameters must be numeric.")
            
        self.cfg['param']['autothreshold'] =  kwargs['thresholdtype'] == 'True' 
        http.refresh(self.base_url + 'run?key=%s' % self.key)
        return self.tmpl_out("wait.html")

    @cherrypy.expose
    @init_app
    def run(self):
        """
        algo execution
        """
        # read the parameters
        t = self.cfg['param']['t']
        m = self.cfg['param']['m']
        autothreshold = self.cfg['param']['autothreshold'] 
        # run the algorithm
        try:
            self.run_algo(t, m, autothreshold)
        except TimeoutError:
            return self.error(errcode='timeout') 
        except RuntimeError:
            return self.error(errcode='runtime')    
        except ValueError:
            return self.error(errcode='badparams',
                              errmsg="The parameters given produce no contours,\
                                     please change them.")

        http.redir_303(self.base_url + 'result?key=%s' % self.key)

        # archive
        if self.cfg['meta']['original']:
            ar = self.make_archive()
            ar.add_file("input_0.png", "original.png", info="uploaded")
            ar.add_file("resu.png", info="output")
            ar.add_file("noiseLevels.txt", info="noise levels")
            ar.add_file("inputContour.txt", info="polygon input")
            ar.add_file("info.txt", info="computation info ")
            ar.add_file("commands.txt", info="commands")
            ar.add_info({"threshold auto": autothreshold}) 
            ar.add_info({"threshold val": t})
            ar.add_info({"contour min size m": m})
            ar.save()

        return self.tmpl_out("run.html")

    def run_algo(self, t, m, autothreshold):
        """
        the core algo runner
        could also be called by a batch processor
        this one needs no parameter
        """
        self.cfg['param']['sizex'] = image(self.work_dir + \
                                           'input_0.png').size[0]
        self.cfg['param']['sizey'] = image(self.work_dir + \
                                            'input_0.png').size[1]
        p = self.run_proc(['convert.sh', 'input_0.png', 'tmp.pgm'])
        self.wait_proc(p, timeout=self.timeout)
        fInput = open(self.work_dir+'tmp.pgm')
        fcommands = open(self.work_dir+"commands.txt", "w")
        f =  open(self.work_dir+"inputContour.txt", "w")
        fInfo =  open(self.work_dir+"info.txt", "w")
        if autothreshold:
            p = self.run_proc(['pgm2freeman', '-min_size', str(m)], stdout=f, \
                              stdin=fInput, stderr=fInfo, \
                              env={'LD_LIBRARY_PATH' : self.bin_dir} )
            self.cfg['param']['commandpgmfreeman'] = 'pgm2freeman  ' +  \
                '-min_size' +str(m) + " < input_0.pgm > inputContourFC.dat \n"
        else :
            p = self.run_proc(['pgm2freeman', '-threshold', str(t), \
                            '-min_size', str(m)],stdout=f, stdin=fInput, \
                             env={'LD_LIBRARY_PATH' : self.bin_dir} )
            self.cfg['param']['commandpgmfreeman'] = 'pgm2freeman ' + \
                    '-threshold' + str(t) +  '-min_size' +str(m) + \
                    " < tmp.pgm > inputContourFC.dat \n" 
        fcommands.write(self.cfg['param']['commandpgmfreeman'])
        fcommands.write("convert  -brightness-contrast \
                         40x-40 input_0.png input_0BG.png \n")
        fInfo.close()
        self.wait_proc(p, timeout=self.timeout)
        sizeContour = os.path.getsize(self.work_dir+"inputContour.txt")
        if sizeContour == 0 : 
            raise ValueError
        f.close()
        p = self.run_proc(['transformBG.sh'])
        self.wait_proc(p, timeout=self.timeout)                
   
        f = open(self.work_dir+"info.txt", "a")
    

        self.cfg['param']['commandms'] = 'meaningfulScaleEstim ' + \
                                        '-enteteXFIG '+ '-drawXFIGNoiseLevel '+\
                                        '-setFileNameFigure '+ \
                                        'noiseLevel.fig '+ '-drawContourSRC '+ \
                                        '4 ' +'1 ' + '-afficheImage '+  \
                                        'input_0BG.png ' + \
                                         str(image(self.work_dir + \
                                        'input_0BG.png').size[0]) +  \
                                         " "+ str(image(self.work_dir + \
                                        'input_0BG.png').size[1]) + \
                                        " -setPosImage " +"1 "+ "1 " + \
                                        "-printNoiseLevel -processAllContours"+\
                                        " > resultNoiseLevel.txt < " +\
                                        "inputContourFC.dat \n"

        fcommands.write(self.cfg['param']['commandms'])
        fInput = open(self.work_dir+'inputContour.txt')
        p = self.run_proc(['applyMS.sh', '-enteteXFIG', '-drawXFIGNoiseLevel', \
                        '-setFileNameFigure', 'noiseLevel.fig', \
                        '-drawContourSRC','4', '1', "-afficheImage", \
                        str(self.work_dir) + 'input_0BG.png', \
                        str(image(self.work_dir + 'input_0BG.png').size[0]), \
                        str(image(self.work_dir + 'input_0BG.png').size[1]), \
                        "-setPosImage", "1", "1", "-printNoiseLevel", \
                        "-processAllContours"] ,  stdin=fInput,stderr=f, \
                        env={'LD_LIBRARY_PATH' : self.bin_dir})
        self.wait_proc(p, timeout=self.timeout)        
        fcommands.write("convert -density 300 resu.eps resu.png \n")
        p = self.run_proc(['convertFig.sh','noiseLevel.fig'], stderr=f)
        self.wait_proc(p, timeout=self.timeout)
        f.close()
        fcommands.close()
        return

    @cherrypy.expose
    @init_app
    def result(self, public=None):
        """
        display the algo results
        """
        return self.tmpl_out("result.html", 
                             height=image(self.work_dir
                                          + 'input_0.png').size[1])
