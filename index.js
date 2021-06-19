const fs = require('fs')
const { spawn } = require('child_process')
const { promisify } = require('util')
const sharp = require('sharp')
const { Writable } = require('stream')


void async function main() {
    const image = 'image.jpg'

    const { data: imageBuffer, info: imageInfo } = await sharp(image)
        .removeAlpha()
        .toBuffer({ resolveWithObject: true })

    if (imageInfo.channels !== 3) {
        throw new Error(`Invalid number of channels`)
    }

    const mask = 'mask.png'
    const { data: maskBuffer, info: maskInfo } = await sharp(mask).toColorspace('b-w').toBuffer({ resolveWithObject: true })

    if (maskInfo.width !== imageInfo.width || maskInfo.height !== imageInfo.height) {
        throw new Error(`Invalid mask dimensions`)
    }

    const cli = spawn('./cli')
    cli.stdin.setDefaultEncoding('binary')
    const output = new BufferWritable(imageInfo.width * imageInfo.height * 3)
    cli.stdout.pipe(output)
    output.once('finish', () => {
        sharp(output.buffer, { raw: {
            channels: 3,
            width: imageInfo.width,
            height: imageInfo.height
        }}).toFile('output.png')
    })

    const write = promisify(cli.stdin.write).bind(cli.stdin)

    const buffer = Buffer.alloc(4)
    buffer.writeInt32LE(imageInfo.width)
    await write(buffer, 'binary')

    buffer.byteOffset = 0
    buffer.writeInt32LE(imageInfo.height)
    await write(buffer, 'binary')    

    await write(imageBuffer, 'binary')
    await write(maskBuffer, 'binary')

    
}()

class BufferWritable extends Writable {
    constructor(size) {
        super({ defaultEncoding: 'binary' });

        this.buffer = Buffer.alloc(size)
        
        this.offset = 0
        
    }

    _write(chunk, encoding, callback) {                
        if (this.offset + chunk.length > this.size) {
            callback(new Error(`Out of space`))
            return
        }

        chunk.copy(this.buffer, this.offset, 0)
        this.offset += chunk.length


        callback()
    }
}