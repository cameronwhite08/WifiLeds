//
//  ViewController.swift
//  WifiLedsiPhoneApp
//
//  Created by David Pye on 3/27/18.
//  Copyright © 2018 David Pye. All rights reserved.
//

import UIKit
import Alamofire

class ViewController: UIViewController{

 
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func Off(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/off")
        print("Off")
    }
    @IBAction func Confetti(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/confetti")
        print("Confetti")
        
    }

    @IBAction func Sinelon(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/sinelon")
        print("Sinelon")
    }
   
    @IBAction func Rainbow(_ sender: AnyObject) {
        Alamofire.request("http://192.168.1.51/rainbow")
        print("Rainbow")
    }
    
    @IBAction func Juggle(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/juggle")
        print("Juggle")
    }
    
    @IBAction func PulseFade(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/pulseFade")
        print("Pulse Fade")
    }
    @IBAction func BPM(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/bpm")
        print("BPM")
    }
    
    @IBAction func Svedka(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/cyclonfill")
        print("Svedka")
    }
    
    @IBAction func Twinkle(_ sender: UIButton) {
        Alamofire.request("http://192.168.1.51/twinkle")
        print("Twinkle")
    }
}

